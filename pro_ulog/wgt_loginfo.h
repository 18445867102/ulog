#ifndef WGT_LOGINFO_H
#define WGT_LOGINFO_H

#include <QApplication>
#include <QDesktopWidget>
#include <QWidget>
#include <QTableWidget>
#include <QHeaderView>

namespace Ui {
class LogInfoWidget;
}

class LogInfoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LogInfoWidget(QWidget *parent = nullptr);
    Ui::LogInfoWidget *ui;
    QTableWidget *tbw_info;
    QTableWidget *tbw_info_mul;
    QTableWidget *tbw_parameters;
signals:

public slots:

private:
};

#endif // WGT_LOGINFO_H
