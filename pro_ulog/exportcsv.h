#ifndef EXPORTCSV_H
#define EXPORTCSV_H

#include <QString>
#include <QFile>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QByteArray>
#include <QThread>
#include <QDir>
#include <QTextCodec>
#include <QMutex>
#include <QReadWriteLock>
#include <ulog.h>


class ExportCSV: public QThread
{
public:
    typedef QString (ExportCSV::*pfun)(QByteArray *array);
    ExportCSV(QString path = "export_csv_file_default_path.csv");
    ~ExportCSV();

    void CSV_Set_Path(QString path);
    void CSV_Get_Data(ulog *d,uint8_t mode_1_2);
    void CSV_Write1();
    void CSV_Write2();

    QString f_bool(QByteArray *array);
    QString f_char(QByteArray *array);
    QString f_u8_t(QByteArray *array);
    QString f_u16_t(QByteArray *array);
    QString f_u32_t(QByteArray *array);
    QString f_u64_t(QByteArray *array);
    QString f_8_t(QByteArray *array);
    QString f_16_t(QByteArray *array);
    QString f_32_t(QByteArray *array);
    QString f_64_t(QByteArray *array);
    QString f_float(QByteArray *array);
    QString f_double(QByteArray *array);
private:
    QString csv_path1;
    QString csv_path2;

    ulog *log;
    uint64_t row;
    uint16_t stop;
    uint8_t mode;

    pfun p;
    bool create = false;

    void run();
};

#endif // EXPORTCSV_H
