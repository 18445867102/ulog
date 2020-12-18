#include "ulog.h"

using namespace std;

ulog::ulog(){
    ptr_len = 0;
    fpos = 0;
    size = 0;
    array = 0;
    nptr = new uint16_t*[20];
}
string ulog::LOG_LEVEL[8] = {"EMERG","ALERT","CRIT","ERR","WARNING","NOTICE","INFO","DEBUG"};
bool ulog::ulog_open_file(string ulog_file_path)
{
    filebuf *pbuf;
    ifstream ulog_stream;

    if (ulog_file_path.empty())
        return false;

    ulog_stream.open(ulog_file_path, ios_base::binary | ios_base::in );
    if(!ulog_stream.is_open())
        return false;

    pbuf=ulog_stream.rdbuf();
    size=pbuf->pubseekoff (0,ios_base::end,ios_base::in);
    pbuf->pubseekpos (0,ios_base::in);
    buffer=new char[size];
    pbuf->sgetn (buffer,static_cast<int>(size));
    ulog_stream.close();

    return true;
}

void ulog::ulog_analysis_file_header_s()
{
    memcpy(&ulog_file_header.magic, buffer, 7);
    fpos+=7;
    memcpy(&ulog_file_header.magic[7], &buffer[fpos++], 1);
    memcpy(&ulog_file_header.timestamp, &buffer[fpos], 8);
    fpos+=8;
}
void ulog::ulog_analysis_flag_bits_s()
{
    memcpy(&ulog_message_flag_bits.compat_flags, &buffer[fpos], 8);
    fpos+=8;
    memcpy(&ulog_message_flag_bits.incompat_flags, &buffer[fpos], 8);
    fpos+=8;
    memcpy(&ulog_message_flag_bits.appended_offsets, &buffer[fpos], 24);
    fpos+=24;
}

void ulog::ulog_analysis_info_header_s(uint16_t size){
    uint8_t key_len;
    uint16_t value_len;
    char *key,*value;

    memcpy(&key_len, &buffer[fpos++], 1);
    value_len = size - 1 - key_len;
    key = new char[key_len+1];
    value = new char[value_len+1];
    memcpy(key, &buffer[fpos], key_len);
    fpos += key_len;
    memcpy(value, &buffer[fpos], value_len);
    fpos += value_len;
    key[key_len]='\0';
    value[value_len]='\0';
    for(int i=0; i<key_len; i++ ){
        if(key[i] == ' '){
            ulog_info_map.insert(pair<string,string>(&key[i+1], value));
            break;
        }
    }
    delete []key;
    delete []value;
}

void ulog::ulog_analysis_format_s(uint16_t size){
    char *format,*message,*type,*field;
    uint8_t len = 0;
    uint16_t id = 0;
    uint32_t id_count;
    uint8_t t_size;
    static uint8_t nid = 0;
    multimap<string, uint16_t>::iterator iter;
    uint8_t mulid = 0;

    do{
        format = new char[size];
        memcpy(format, &buffer[fpos], size);
        fpos += size;
        message = format;
        for(int i=0,k=3; i<size; i++ ){
            if(format[i] == ';'){
                format[i] = '\0';
                if("_padding0" == string(field)){
                    ptr[id][2] -= len;
                    len = 0;
                    type = &format[i+1];
                    continue;
                }
                t_size = _sizeof(type);
                if(!len){
                    if(t_size != 10){
                        ulog_msgname_type.insert(pair<string,string>
                                               (string(message)+"_"+to_string(ptr[id][1])+"."+string(field),type));
                        ulog_msgname_num.insert(pair<string,uint16_t>
                                               (string(message)+"_"+to_string(ptr[id][1])+"."+string(field),array++));
                        ptr[id][k++] = t_size;
                    }else{
                        ptr[id][k++] = t_size + nid;
                        ulog_nest_format(string(message)+"_"+to_string(ptr[id][1]), field,type,nid);
                        nid++;
                    }
                }else{
                    if(t_size != 10){
                        for(int j=0; j<len; j++,k++){
                            ulog_msgname_type.insert(pair<string,string>
                                                   (string(message)+"_"+to_string(ptr[id][1])+"."+string(field)+
                                                    '['+to_string(j)+']',type));
                            ulog_msgname_num.insert(pair<string,uint16_t>
                                                   (string(message)+"_"+to_string(ptr[id][1])+"."+string(field)+
                                                    '['+to_string(j)+']',array++));
                            ptr[id][k] = t_size;
                        }
                    }else{
                        for(int j=0; j<len; j++,k++){
                            ulog_nest_format(string(message)+"_"+to_string(ptr[id][1]), string(field)+'['+to_string(j)+']',type,nid);
                            ptr[id][k] = t_size + nid;
                        }
                        nid++;
                    }
                    len = 0;
                }
                type = &format[i+1];
            }else if(format[i] == ':'){
                format[i] = '\0';
                type = &format[i+1];
                id_count = ulog_msgname_id.count(message);
                if(!id_count)
                    goto next2;
                else if(id_count==1){
                    iter = ulog_msgname_id.find(message);
                    id = iter->second;
                }else{
                    iter = ulog_msgname_id.find(message);
                    for(int m=0;m<mulid;m++)
                        iter++;
                    id = iter->second;
                    if(mulid != id_count-1){
                        fpos -= (size+3);
                        mulid++;
                    }else
                        mulid = 0;
                }
                ptr[id][0] = array;
            }else if(format[i] == ' '){
                format[i] = '\0';
                field = &format[i+1];
            }else if(format[i] == '['){
                format[i++] = '\0';
                for(int j=0; format[i]!=']'; j++, i++)
                    len = len*10 + static_cast<uint8_t>(format[i]%16);
            }
        }
        next2:
        delete []format; 
        memcpy(&size, &buffer[fpos], 2);
        fpos += 2;
    }while(buffer[fpos++] == 'F');

    fpos -= 3;

    ulog_data = new QByteArray[array];
    //ulog_data = new vector<char>[array];
}
void ulog::ulog_nest_format(string msg, string _msg,string mtype, uint8_t nid){
    uint64_t pos;
    uint16_t size;
    char *format = nullptr;
    char *message = nullptr;
    char *type = nullptr;
    char *field = nullptr;
    uint8_t count =0;
    uint8_t len;

    pos = 16;
    beijing:
    memcpy(&size, &buffer[pos], 2);
    pos +=2;
    if(buffer[pos++] != 'F'){
        pos += size;
        goto beijing;
    }
    uint64_t t_pos = pos;
    uint16_t t_size = size;
    do{
        for(uint32_t i=0; i<size; i++ ){
            if(buffer[pos+i] == ':'){
                message = new char[i+1];
                memcpy(message, &buffer[pos], i);
                message[i] = '\0';
                if(message != mtype)
                    goto haicheng;
            }else if(buffer[pos+i] == ';')
                count++;
            else if(buffer[pos+i] == '['){
                len = 0;
                for(;buffer[pos+i+1]!=']';i++)
                    len = len*10 + static_cast<uint8_t>(buffer[pos+i+1]%16);
                count += (len-1);
            }
        }
        nptr[nid] = new uint16_t[count+1];
        nptr[nid][0] = count;
        if(message != nullptr)
            delete []message;
        goto xiliu;
        haicheng:
        if(message != nullptr)
            delete []message;
        pos += size;
        memcpy(&size, &buffer[pos], 2);
        pos += 3;
    }while (1);
    xiliu:
    pos = t_pos;
    size = t_size;
    len = 0;
    do{
        format = new char[size];
        memcpy(format, &buffer[pos], size);
        pos += size;
        message = format;
        for(int i=0,k=1; i<size; i++){
            if(format[i] == ';'){
                format[i] = '\0';
                if("_padding0" == string(field)){
                    nptr[nid][0] -= len;
                    len = 0;
                    type = &format[i+1];
                    continue;
                }
                t_size = _sizeof(type);
                if(!len){
                    ulog_msgname_type.insert(pair<string,string> (string(msg)+"."+string(_msg)+"."+string(field),type));
                    ulog_msgname_num.insert(pair<string,uint16_t> (string(msg)+"."+string(_msg)+"."+string(field),array++));
                    nptr[nid][k++] = t_size;
                }else{
                    for(int j=0; j<len; j++,k++){
                        ulog_msgname_type.insert(pair<string,string>(string(msg)+"."+string(_msg)+"."+string(field)+'['+to_string(j)+']',type));
                        ulog_msgname_num.insert(pair<string,uint16_t> (string(msg)+"."+string(_msg)+"."+string(field)+'['+to_string(j)+']',array++));
                        nptr[nid][k] = t_size;
                    }
                    len = 0;
                }
                type = &format[i+1];
            }else if(format[i] == ':'){
                format[i] = '\0';
                if(message != mtype)
                    goto next3;
                type = &format[i+1];
            }else if(format[i] == ' '){
                format[i] = '\0';
                field = &format[i+1];
            }else if(format[i] == '['){
                format[i++] = '\0';
                for(int j=0; format[i]!=']'; j++, i++)
                    len = len*10 + static_cast<uint8_t>(format[i]%16);
            }
        }
        delete []format;
        return;
        next3:
        delete []format;
        memcpy(&size, &buffer[pos], 2);
        pos += 3;
    }while (1);
}
void ulog::ulog_get_id_msgname(){
    int64_t pos = 16;
    uint16_t msg_size, msg_len, msg_id;
    uint8_t multi_id;
    char *msg_name;

    nehe:
    memcpy(&msg_size, &buffer[pos], 2);
    pos +=2;
    if(buffer[pos++] != 'A'){
        pos += msg_size;
        goto nehe;
    }
    pos -=3;

    while(pos < this->size){
        memcpy(&msg_size, &buffer[pos], 2);
        pos +=2;
        if(buffer[pos++] == 'A'){
            ptr_len++;
            msg_len = msg_size - 3;
            memcpy(&multi_id, &buffer[pos++], 1);
            memcpy(&msg_id, &buffer[pos], 2);
            pos += 2;
            msg_name = new char[msg_len+1];
            memcpy(msg_name, &buffer[pos], msg_len);
            msg_name[msg_len]='\0';
            ulog_msgname_id.insert(pair<string,uint16_t>(msg_name,msg_id));
            ulog_id_multid.insert(pair<uint16_t,uint16_t>(msg_id, multi_id));
            delete []msg_name;
            pos += msg_len;
        }else
            pos += msg_size;
    }

    ptr = new uint16_t*[ptr_len];
}
void ulog::ulog_get_str_member(){
    uint16_t msg_size,id;
    uint32_t temp;
    uint16_t type_count;
    char *message = nullptr;
    multimap<string, uint16_t>::iterator iter;

    fpos += 16;
    here2:
    memcpy(&msg_size, &buffer[fpos], 2);
    fpos +=2;
    if(buffer[fpos++] != 'F'){
        fpos += msg_size;
        goto here2;
    }
    do{
        type_count = 0;
        for(uint32_t i=0; i<msg_size; i++ ){
            if(buffer[fpos+i] == ':'){
                message = new char[i+1];
                memcpy(message, &buffer[fpos], i);
                message[i] = '\0';
                if(!ulog_msgname_id.count(message))
                    goto harbin;
            }else if(buffer[fpos+i] == ';')
                type_count++;
            else if(buffer[fpos+i] == '['){
                id = 0;
                for(;buffer[fpos+i+1]!=']';i++)
                    id = id*10 + static_cast<uint8_t>(buffer[fpos+i+1]%16);
                type_count += (id-1);
            }
        }

        temp = ulog_msgname_id.count(message);
        iter = ulog_msgname_id.find(message);
        for(uint32_t i=0; i<temp; i++,iter++){
            id = iter->second;
            ptr[id] = new uint16_t[type_count+3];
            ptr[id][1] = ulog_id_multid.find(id)->second;
            ptr[id][2] = type_count;
        }

        harbin:
        if(message != nullptr)
            delete []message;
        fpos += msg_size;
        memcpy(&msg_size, &buffer[fpos], 2);
        fpos += 2;
    }while (buffer[fpos++] == 'F');
    fpos = 0;
}
void ulog::ulog_analysis_parameter_header_s(){
    uint8_t key_len;
    char *key;
    float value;

    memcpy(&key_len, &buffer[fpos++], 1);
    key = new char[key_len+1];
    memcpy(key, &buffer[fpos], key_len);
    fpos += key_len;
    memcpy(&value, &buffer[fpos], 4);
    fpos += 4;
    key[key_len]='\0';
    for(int i=0; i<key_len; i++ ){
        if(key[i] == ' '){
            ulog_parameter_map.insert(pair<string,string>
                                      (&key[i+1], to_string(value)));
            break;
        }
    }
    delete []key;
}
void ulog::ulog_analysis_info_multiple_header_s(uint16_t size){
    uint8_t key_len, is_continued;
    uint16_t value_len;
    char *key,*value;

    memcpy(&is_continued, &buffer[fpos++], 1);
    memcpy(&key_len, &buffer[fpos++], 1);
    value_len = size - 2 - key_len;
    key = new char[key_len+1];
    value = new char[value_len+1];
    memcpy(key, &buffer[fpos], key_len);
    fpos += key_len;
    memcpy(value, &buffer[fpos], value_len);
    fpos += value_len;
    key[key_len]='\0';
    value[value_len]='\0';
    for(int i=0; i<value_len; i++ ){
        if(value[i] == ':'){
            value[i+1] = '\0';
            ulog_multi_info_map.insert(pair<string,string>
                                       (value, &value[i+2]));
            break;
        }
    }
    delete []key;
    delete []value;
}
void ulog::ulog_analysis_add_logged_s(uint16_t size)
{
    fpos += size;
}
uint8_t ulog::_sizeof(string type){
    if(type =="float") return 4;
    else if(type == "bool") return 1;
    else if(type == "uint8_t")return 1;
    else if(type == "uint16_t")return 2;
    else if(type == "uint32_t")return 4;
    else if(type == "uint64_t")return 8;
    else if(type == "double")return 8;
    else if(type == "int8_t")return 1;
    else if(type == "int16_t")return 2;
    else if(type == "int32_t")return 4;
    else if(type == "int64_t")return 8;
    else if(type == "char")return 1;
    else return 10;
}
void ulog::ulog_analysis_data_s(){
    uint16_t msg_id;

    memcpy(&msg_id, &buffer[fpos], 2);
    fpos += 2;

    uint16_t count = ptr[msg_id][2];
    uint16_t pos = ptr[msg_id][0];
    uint16_t len;

    for(uint16_t i = 0; i < count; i++){
        len = ptr[msg_id][i+3];
        if(len >= 10){
            uint16_t len2;
            for(uint8_t j=0; j<nptr[len-10][0];j++){
                len2 = nptr[len-10][j+1];
                /********************************************/
                ulog_data[pos+i+j].append(buffer+fpos,len2);
                /********************************************/
                //for(uint32_t k=0;j<len2;k++)
                    //ulog_data[pos+i+j].push_back(*(buffer+fpos+k));
                /********************************************/
                fpos += len2;
            }
        } else {
            /********************************************/
            //for(uint8_t j=0;j<len;j++)
                //ulog_data[pos+i].push_back(*(buffer+fpos+j));
            /********************************************/
            ulog_data[pos+i].append(buffer+fpos,len);
            /********************************************/
            fpos += len;
        }
    }
}
void ulog::ulog_analysis_message_logging_s(uint16_t size){
    uint32_t msg_len = size - 9;
    uint8_t log_level;
    uint64_t timestamp;
    char* message;

    memcpy(&log_level, &buffer[fpos++], 1);
    memcpy(&timestamp, &buffer[fpos], 8);
    fpos+=8;
    message = new char[msg_len+1];
    memcpy(message, &buffer[fpos], msg_len);
    fpos+=msg_len;
    message[msg_len] = '\0';
    ulog_loggings.insert(pair<uint64_t,string>(timestamp, LOG_LEVEL[log_level%8]));
    ulog_loggings.insert(pair<uint64_t,string>(timestamp, message));

    delete []message;
}
void ulog::run(){
    ulog_get_id_msgname();
    ulog_get_str_member();
    ulog_analyze();

}

bool ulog::ulog_analyze(){
    uint16_t msg_size;
    char msg_type;

    ulog_analysis_file_header_s();

    while (fpos < size){
        memcpy(&msg_size, &buffer[fpos], 2);
        fpos+=2;
        msg_type = buffer[fpos++];
        //cout<<msg_type<<endl;
        switch (msg_type) {
        case 'I':
           ulog_analysis_info_header_s(msg_size);
           break;
        case 'D':
           ulog_analysis_data_s();
           break;
        case 'F':
           ulog_analysis_format_s(msg_size);
           break;
        case 'M':
           ulog_analysis_info_multiple_header_s(msg_size);
           break;
        case 'P':
           ulog_analysis_parameter_header_s();
           break;
        case 'A':
           ulog_analysis_add_logged_s(msg_size);
           break;
        case 'R':
           fpos+=msg_size;
           break;
        case 'S':
           fpos+=msg_size;
           break;
        case 'O':
           fpos+=msg_size;
           break;
        case 'L':
           ulog_analysis_message_logging_s(msg_size);
           break;
        case 'B':
           ulog_analysis_flag_bits_s();
           break;
        default:
           fpos++;
           cout<<"fail."<<endl;
           break;
        }
    }
    delete []buffer;
    return true;
}
ulog::~ulog(){
    for(int i=0; i<ptr_len; i++)
        delete []ptr[i];
    delete []ptr;
    delete []nptr;
}
