#include "exportcsv.h"

ExportCSV::ExportCSV(QString path)
{      
   csv_path1 = path;
   stop = 0;
   row = 0;
}

void ExportCSV::CSV_Set_Path(QString path)
{
    csv_path1 = path.replace(QString("ulg"), QString("csv"),Qt::CaseInsensitive);
    csv_path2 = path.remove(".csv");
}

void ExportCSV::CSV_Get_Data(ulog *log,uint8_t mode_1_2){
    if(log == nullptr)
        return;
    this->log = log;

    mode = mode_1_2;
    start();
}
void ExportCSV::CSV_Write1()
{
    QFile file(csv_path1);
    QTextStream stream;
    uint16_t array_len = log->array;

    if(!file.open(QIODevice::ReadWrite|QIODevice::Truncate)){
        qDebug()<<"create fail.";
        create = false;
        return;
    }
    create = true;
    stream.setDevice(&file);
    qDebug()<<"create success!";

    QMap<QString,uint16_t> name_num;
    map<string, uint16_t>::iterator iter =log->ulog_msgname_num.begin();
    while(iter != log->ulog_msgname_num.end()){
        name_num.insert(iter->first.c_str(),iter->second);
        iter++;
    }

    for(int i=0; i<array_len; i++)
        stream<<name_num.key(i)+",";
    stream<<"\n";

    /* 遍历ulog_format_map根据数据类型将对应函数名称存入对应角标的函数指针数组 */
    pfun fun[array_len];
    map<string, string>::iterator i =log->ulog_msgname_type.begin();
    while(i != log->ulog_msgname_type.end()){
        QString a = i->second.c_str();
        int b =name_num.value(i->first.c_str());
        //qDebug()<<a<<b;
        if(!QString::compare("float", a)){
            fun[b] = &ExportCSV::f_float;
        }else if(!QString::compare("uint8_t", a)){
            fun[b] = &ExportCSV::f_u8_t;
        }else if(!QString::compare("uint32_t",a)){
            fun[b] = &ExportCSV::f_u32_t;
        }else if(!QString::compare("uint64_t", a)){
            fun[b] = &ExportCSV::f_u64_t;
        }else if(!QString::compare("double",a)){
            fun[b] = &ExportCSV::f_double;
        }else if(!QString::compare("int32_t", a)){
            fun[b] = &ExportCSV::f_32_t;
        }else if(!QString::compare("int64_t", a)){
            fun[b] = &ExportCSV::f_64_t;
        }else if(!QString::compare("int8_t", a)){
            fun[b] = &ExportCSV::f_8_t;
        }else if(!QString::compare("char", a)){
            fun[b] = &ExportCSV::f_char;
        }else if(!QString::compare("bool", a)){
            fun[b] = &ExportCSV::f_bool;
        }else if(!QString::compare("uint16_t", a)){
            fun[b] = &ExportCSV::f_u16_t;
        }else if(!QString::compare("int16_t", a)){
            fun[b] = &ExportCSV::f_16_t;
        }else
            qDebug()<<"error----------------------";
        i++;
    }

    /* 当数据点数超过数组包含得点数时，stop++,当stop=array_len时，说明所用数据写入完毕，退出while循环 */
    while(stop != array_len){
        stop = 0;
        /* 循环遍历array 将数据或','写入文件 */
        for(quint16 i=0; i<array_len; i++){
            p = fun[i];/* 赋值函数指针 */
            stream<<(this->*p)(&log->ulog_data[i]);
        }
        /* 每行写完加入换行符，行号加一 */
        stream<<"\n";
        row++;
    }

//***************************************************************//
//    fun[5] = &ExportCSV::f_float;                              //
//    p = fun[5];                                                //
//    stream<<(this->*p)(&log.ulog_data[301],stop,row);          //
//***************************************************************//
    if(file.isOpen())
        file.close();
}
void ExportCSV::CSV_Write2()
{
    uint8_t msg_id;
    uint16_t count,pos,jump;
    QMap<QString,uint16_t> name_num;
    map<string, uint16_t>::iterator iter_num =log->ulog_msgname_num.begin();
    while(iter_num != log->ulog_msgname_num.end()){
        name_num.insert(iter_num->first.c_str(),iter_num->second);
        iter_num++;
    }

    pfun fun[log->array];
    map<string, string>::iterator i =log->ulog_msgname_type.begin();
    while(i != log->ulog_msgname_type.end()){
        QString a = i->second.c_str();
        int b =name_num.value(i->first.c_str());
        if(!QString::compare("float", a)){
            fun[b] = &ExportCSV::f_float;
        }else if(!QString::compare("uint8_t", a)){
            fun[b] = &ExportCSV::f_u8_t;
        }else if(!QString::compare("uint32_t",a)){
            fun[b] = &ExportCSV::f_u32_t;
        }else if(!QString::compare("uint64_t", a)){
            fun[b] = &ExportCSV::f_u64_t;
        }else if(!QString::compare("double",a)){
            fun[b] = &ExportCSV::f_double;
        }else if(!QString::compare("int32_t", a)){
            fun[b] = &ExportCSV::f_32_t;
        }else if(!QString::compare("int64_t", a)){
            fun[b] = &ExportCSV::f_64_t;
        }else if(!QString::compare("int8_t", a)){
            fun[b] = &ExportCSV::f_8_t;
        }else if(!QString::compare("char", a)){
            fun[b] = &ExportCSV::f_char;
        }else if(!QString::compare("bool", a)){
            fun[b] = &ExportCSV::f_bool;
        }else if(!QString::compare("uint16_t", a)){
            fun[b] = &ExportCSV::f_u16_t;
        }else if(!QString::compare("int16_t", a)){
            fun[b] = &ExportCSV::f_16_t;
        }else
            qDebug()<<"error----------------------";
        i++;
    }

    multimap<string,uint16_t>::iterator iter = log->ulog_msgname_id.begin();
    while(iter != log->ulog_msgname_id.end()){
        //遍历消息结构判定是否有数据
        msg_id = iter->second;
        count = log->ptr[msg_id][2];
        pos = log->ptr[msg_id][0];
        jump = 0;
        for(uint16_t i=0; i<count; i++){
            if(log->ulog_data[pos+i].size() == 0)
                jump++;
        }
        if(jump == count){
            iter++;
            continue;
        }
        //根据消息名创建CSV文件
        QFile file(csv_path2+'_'+QString(iter->first.c_str())+'_'+QString::number(log->ptr[msg_id][1])+".csv");
        if(!file.open(QIODevice::ReadWrite|QIODevice::Truncate)){
            qDebug()<<"create fail.";
            iter++;
            continue;
        }
        QTextStream stream(&file);

        //写入数据类型名字
        for(uint16_t i = pos; i<pos+count; i++){
            QString s =name_num.key(i);
            stream<<s.remove(QString(iter->first.c_str())+": ")+",";
        }
        stream<<"\n";

        //写入数据
        while(stop != count){
            stop = 0;
            for(uint16_t i = pos; i<pos+count; i++){//遍历所有数组
                p = fun[i];
                stream<<(this->*p)(&log->ulog_data[i]);
            }
            stream<<"\n";
            row++;
        }
        stop = 0;
        row=0;
        if(file.isOpen())
            file.close();
        iter++;
    }
}
void ExportCSV::run(){
    if(mode == 1)
        CSV_Write1();
    else if(mode ==2)
        CSV_Write2();
}

QString ExportCSV::f_float(QByteArray *array){
    float d;
    if((row<<2)<(uint32_t)array->size()){
        memcpy(&d,&array->data()[row<<2],4);
        return QString("%1").arg(d)+",";
    }else{
        stop++;
        return ",";
    }
}

QString ExportCSV::f_bool(QByteArray *array){
    uint8_t d;
    if(row<(uint32_t)array->size()){
        memcpy(&d,&array->data()[row],1);
        return QString::number(d)+",";
    }else{
        stop++;
        return ",";
    }
}
QString ExportCSV::f_char(QByteArray *array){
    char d;
    if(row<(uint32_t)array->size()){
        memcpy(&d,&array->data()[row],1);
        return QString::number(d)+",";
    }else{
        stop++;
        return ",";
    }
}
QString ExportCSV::f_u8_t(QByteArray *array){
    uint8_t d;
    if(row<(uint32_t)array->size()){
        memcpy(&d,&array->data()[row],1);
        return QString::number(d)+",";
    }else{
        stop++;
        return ",";
    }
}
QString ExportCSV::f_u16_t(QByteArray *array){
    uint16_t d;
    if((row<<1)<(uint32_t)array->size()){
        memcpy(&d,&array->data()[row<<1],2);
        return QString::number(d)+",";
    }else{
        stop++;
        return ",";
    }
}
QString ExportCSV::f_u32_t(QByteArray*array){
    uint32_t d;
    if((row<<2)<(uint32_t)array->size()){
        memcpy(&d,&array->data()[row<<2],4);
        return QString::number(d)+",";
    }else{
        stop++;
        return ",";
    }
}
QString ExportCSV::f_u64_t(QByteArray *array){
    uint64_t d;
    if((row<<3)<(uint32_t)array->size()){
        memcpy(&d,&array->data()[row<<3],8);
        return QString::number(d)+",";
    }else{
        stop++;
        return ",";
    }
}
QString ExportCSV::f_8_t(QByteArray *array){
    int8_t d;
    if(row<(uint32_t)array->size()){
        memcpy(&d,&array->data()[row],1);
        return QString::number(d)+",";
    }else{
        stop++;
        return ",";
    }
}
QString ExportCSV::f_16_t(QByteArray *array){
    int16_t d;
    if((row<<1)<(uint32_t)array->size()){
        memcpy(&d,&array->data()[row<<1],2);
        return QString::number(d)+",";
    }else{
        stop++;
        return ",";
    }
}
QString ExportCSV::f_32_t(QByteArray *array){
    int32_t d;
    if((row<<2)<(uint32_t)array->size()){
        memcpy(&d,&array->data()[row<<2],4);
        return QString::number(d)+",";
    }else{
        stop++;
        return ",";
    }
}
QString ExportCSV::f_64_t(QByteArray *array){
    int64_t d;
    if((row<<3)<(uint32_t)array->size()){
        memcpy(&d,&array->data()[row<<3],8);
        return QString::number(d)+",";
    }else{
        stop++;
        return ",";
    }
}
QString ExportCSV::f_double(QByteArray *array){
    double d;
    if((row<<3)<(uint32_t)array->size()){
        memcpy(&d,&array->data()[row<<3],8);
        return QString::number(d,'f',10)+",";
    }else{
        stop++;
        return ",";
    }
}
ExportCSV::~ExportCSV(){}
