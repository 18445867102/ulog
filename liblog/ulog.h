#ifndef ULOG_H
#define ULOG_H

#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned int        uint32_t;
typedef unsigned long long  uint64_t;
typedef signed char         int8_t;
typedef short               int16_t;
typedef int                 int32_t;
typedef long long           int64_t;

enum class ULogMessageType : uint8_t {
    FORMAT = 'F',
    DATA = 'D',
    INFO = 'I',
    INFO_MULTIPLE = 'M',
    PARAMETER = 'P',
    ADD_LOGGED_MSG = 'A',
    REMOVE_LOGGED_MSG = 'R',
    SYNC = 'S',
    DROPOUT = 'O',
    LOGGING = 'L',
    FLAG_BITS = 'B',
};

struct ulog_file_header_s {
    uint8_t magic[8];
    uint64_t timestamp;
};
struct ulog_message_header_s {
    uint16_t msg_size;
    uint8_t msg_type;
};
struct ulog_message_remove_logged_s {
    uint16_t msg_size;
    uint8_t msg_type = static_cast<uint8_t>(ULogMessageType::REMOVE_LOGGED_MSG);
    uint16_t msg_id;
};
struct ulog_message_sync_s {
    uint16_t msg_size;
    uint8_t msg_type = static_cast<uint8_t>(ULogMessageType::SYNC);
    uint8_t sync_magic[8];
};
struct ulog_message_dropout_s {
    uint16_t msg_size = sizeof(uint16_t);
    uint8_t msg_type = static_cast<uint8_t>(ULogMessageType::DROPOUT);
    uint16_t duration;
};
struct ulog_message_data_header_s {
    uint16_t msg_size;
    uint8_t msg_type = static_cast<uint8_t>(ULogMessageType::DATA);
    uint16_t msg_id;
};
struct ulog_message_flag_bits_s {
    uint16_t msg_size;
    uint8_t msg_type = static_cast<uint8_t>(ULogMessageType::FLAG_BITS);
    uint8_t compat_flags[8];
    uint8_t incompat_flags[8];
    uint64_t appended_offsets[3];
};

class ulog: public QThread
{
public:
    ulog();
    ~ulog();

    uint8_t _sizeof(string type);
    void ulog_nest_format(string msg, string _msg,string type, uint8_t npos);
    void ulog_get_id_msgname();
    void ulog_get_str_member();
    void ulog_analysis_file_header_s();
    void ulog_analysis_message_logging_s(uint16_t size);
    void ulog_analysis_flag_bits_s();
    void ulog_analysis_info_header_s(uint16_t size);
    void ulog_analysis_info_multiple_header_s(uint16_t size);
    void ulog_analysis_format_s(uint16_t size);
    void ulog_analysis_parameter_header_s();
    void ulog_analysis_add_logged_s(uint16_t size);
    void ulog_analysis_data_s();

    /* 读取日志，解析日志 */
    bool ulog_open_file(string ulog_file_path);
    bool ulog_analyze();

    /* 以下容器存储解析后的所有数据 */
    ulog_file_header_s          ulog_file_header;       //存储日志文件头部信息
    ulog_message_flag_bits_s    ulog_message_flag_bits; //存储文件标记位信息
    map<string, string>         ulog_info_map;          //info
    map<string, string>         ulog_multi_info_map;    //multi info
    map<string, string>         ulog_parameter_map;     //parameter
    map<string, string>         ulog_msgname_type;      //key:字段名称   value:数据类型
    map<string, uint16_t>       ulog_msgname_num;       //key:字段名称   value:对应ulog_data的索引值
    map<uint16_t, uint16_t>     ulog_id_multid;         //key:id        value:multi id
    multimap<string,uint16_t>   ulog_msgname_id;        //key:消息名     value:id号（一个消息可能会有多个id）
    multimap<string, string>    ulog_format_map;        //key:字段名称   value:1.数据类型 2.存储数据容器的索引值
    multimap<uint64_t, string>  ulog_loggings;          //key:UTC时间    value:1.级别 2.打印的消息内容
    vector<char> *ulog_data;


    uint16_t array;
    uint16_t **ptr = nullptr;
    uint16_t **nptr = nullptr;
    uint16_t ptr_len;
private:
    char * buffer;
    int64_t fpos, size;
    static string LOG_LEVEL[8];
protected:
    void run();
};

#endif // ULOG_H
