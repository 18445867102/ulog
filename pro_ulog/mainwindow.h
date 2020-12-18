#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//Qt
#include <QMainWindow>
#include <QWidget>
#include <QLabel>
#include <QToolButton>
#include <QFileDialog>
#include <QTextCodec>
#include <QDebug>
#include <QStringList>
#include <QComboBox>
#include <QTableWidget>
#include <QHeaderView>
#include <QDateTimeAxis>

//My
#include "ui_mainwindow.h"
#include "wgt_logfield.h"
#include "wgt_loginfo.h"
#include "wgt_logmsg.h"
#include "ulog.h"
#include "exportcsv.h"
#include "mchartview.h"
#include "mitem.h"
#include "mslider.h"

//C++
#include <map>
#include <iterator>

#include <QtCharts>
using namespace QtCharts;

class MainWindow: public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void create_toolbar();
    void create_chart();
    void readSettings();
    void writeSettings();

    MChartView *chart_view;
private:
    //文件打开标志
    bool isOpen = false;
    bool timeType = false;
    QMap<QString, QString>  msgname_type;
    QMap<QString, uint16_t> msgname_num;

    ulog *mlog = nullptr;
    ExportCSV *csv = nullptr;
    //子窗口
    FieldlistWidget *fieldlist_widget;
    LogInfoWidget *loginfo_widget;
    LoggingsWidget *loggings_widget;

    Ui_MainWindow *ui;

    //底部状态栏显示信息
    QLabel *open;
    QLabel *stb_msg_lable;
    QLabel *stb_point_lab;

    //工具栏部件
    QLabel *tba_lab_myset;
    QComboBox *tba_cob_setname;
    QCheckBox *tba_chk_data_mark;
    QToolButton *tba_btn_save;
    QToolButton *tba_btn_delete;
    QToolButton *tba_btn_logmessage;
    QToolButton *tba_btn_open_log;
    QToolButton *tba_btn_word_list;
    QToolButton *tba_btn_log_info;

    //左侧子窗口
    QTableWidget *tbw_plot;

    //折线图
    QChart *chart;
    QDateTimeAxis *timeAxisX;
    QValueAxis *axisX;
    QValueAxis *axisY;
    QDateTime time;

    void closeEvent(QCloseEvent *event);
    void changeEvent(QEvent *event);
public slots:
    void open_log();
    void open_fieldlist_widget();
    void open_loginfo_widget();
    void open_logmessage_widget();
    void slot_change_fieldlist(QString);
    void slot_add_field();
    void solt_mouseMove(QPoint);
    void solt_export_csv_file1();
    void solt_export_csv_file2();
    void solt_btn_removeall();
    void solt_btn_remove();
    void solt_ulog_finished();
    void solt_itemclick(QTableWidgetItem*);
    void solt_itemDoubleClicked(QTableWidgetItem*);
    void solt_plotDoubleClicked(QTableWidgetItem*);
    void solt_sliderMoved(int);
    void solt_setBootTime(bool);
    void solt_setUtcTime(bool);
    void solt_setAnimation(int);
    void solt_setTheme(int);
};

class ItemDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    ItemDelegate()
    {
    }
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        QStyleOptionViewItem  viewOption(option);
        //高亮显示与普通显示时的前景色一致（即选中行和为选中时候的文字颜色一样）
        viewOption.palette.setColor(QPalette::HighlightedText, index.data(Qt::ForegroundRole).value<QColor>());
        QItemDelegate::paint(painter, viewOption, index);
    }
};
#endif // MAINWINDOW_H
