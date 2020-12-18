#ifndef WGT_LOGMESSAGE_H
#define WGT_LOGMESSAGE_H

#include <QApplication>
#include <QDesktopWidget>
#include <QWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QVBoxLayout>

class LoggingsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LoggingsWidget(QWidget *parent = nullptr);

    QTableWidget *tbw_logMessage;

signals:

public slots:
};

#endif
// WGT_LOGMESSAGE_H
