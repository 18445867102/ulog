#-------------------------------------------------
#
# Project created by QtCreator 2018-11-22T17:22:56
#
#-------------------------------------------------

QT       += core gui
QT       += charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SUNWIN_ULOG_1_1
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    wgt_loginfo.cpp \
    ulog.cpp \
    mchartview.cpp \
    exportcsv.cpp \
    wgt_logmsg.cpp \
    wgt_logfield.cpp \
    mitem.cpp \
    mslider.cpp

HEADERS += \
        mainwindow.h \
    wgt_loginfo.h \
    ulog.h \
    mchartview.h \
    exportcsv.h \
    wgt_logmsg.h \
    wgt_logfield.h \
    mitem.h \
    mslider.h

FORMS += \
        mainwindow.ui \
    wgt_loginfo.ui

## Default rules for deployment.
#qnx: target.path = /tmp/$${TARGET}/bin
#else: unix:!android: target.path = /opt/$${TARGET}/bin
#!isEmpty(target.path): INSTALLS += target


RC_FILE =qt.rc

RESOURCES += \
    source.qrc
