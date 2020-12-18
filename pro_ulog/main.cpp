#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
//    QTextCodec *codec = QTextCodec::codecForName("GB18030");
//    QTextCodec::setCodecForLocale(codec);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
