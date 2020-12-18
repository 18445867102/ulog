#include "wgt_logmsg.h"

LoggingsWidget::LoggingsWidget(QWidget *parent) : QWidget(parent)
{
    this->setWindowTitle("Log Message");
    this->setWindowIcon(QPixmap(":/images/app.ico"));
    this->resize(QApplication::desktop()->width()*0.45,QApplication::desktop()->height()*0.5);

    tbw_logMessage = new QTableWidget(this);
    tbw_logMessage->setColumnCount(3);
    tbw_logMessage->setColumnWidth(0, 200);
    tbw_logMessage->setColumnWidth(1, 120);
    tbw_logMessage->setHorizontalHeaderLabels(QStringList()<<"Time"<<"Level"<<"Message");
    tbw_logMessage->setSelectionBehavior(QAbstractItemView::SelectRows);     //设置每次选择一行
    tbw_logMessage->setEditTriggers(QAbstractItemView::NoEditTriggers);      //设置不可编辑
    tbw_logMessage->setStyleSheet("selection-background-color:lightblue;");  //设置选中背景色
    tbw_logMessage->setAlternatingRowColors(true);
    tbw_logMessage->viewport()->setFocusPolicy(Qt::NoFocus);

    QHeaderView *v_headerView = tbw_logMessage->verticalHeader();
    v_headerView->setHidden(true);
    v_headerView->setDefaultSectionSize(20);
    QHeaderView *h_headerView = tbw_logMessage->horizontalHeader();
    h_headerView->setStretchLastSection(true);                          //列最后的空间补全
    h_headerView->setDefaultAlignment(Qt::AlignHCenter);

    QVBoxLayout *v_boxlayout = new QVBoxLayout;
    v_boxlayout->addWidget(tbw_logMessage);
    v_boxlayout->setContentsMargins(2,2,2,2);
    this->setLayout(v_boxlayout);
}
