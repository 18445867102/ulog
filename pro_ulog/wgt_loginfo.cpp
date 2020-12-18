#include "wgt_loginfo.h"
#include "ui_wgt_loginfo.h"

LogInfoWidget::LogInfoWidget(QWidget *parent):
    QWidget(parent),
    ui(new Ui::LogInfoWidget)
{
    ui->setupUi(this);
    ui->splitter_h->setStretchFactor(0,6);
    ui->splitter_h->setStretchFactor(1,4);
    this->setWindowTitle("Log Info");
    this->setWindowIcon(QPixmap(":/images/app.ico"));
    this->resize(QApplication::desktop()->width()*0.5,QApplication::desktop()->height()*0.55);

    //Info Widget
    tbw_info = ui->table_info;
    tbw_info->setColumnCount(2);
    tbw_info->setColumnWidth(0, 130);
    tbw_info->setHorizontalHeaderLabels(QStringList()<<"Property"<<"Value");
    tbw_info->viewport()->setFocusPolicy(Qt::NoFocus);
    tbw_info->setSelectionBehavior(QAbstractItemView::SelectRows);     //设置每次选择一行
    tbw_info->setEditTriggers(QAbstractItemView::NoEditTriggers);      //设置不可编辑
    tbw_info->setStyleSheet("selection-background-color:lightblue;");  //设置选中背景色
    QHeaderView *v_headerView = tbw_info->verticalHeader();
    v_headerView->setHidden(true);
    v_headerView->setDefaultSectionSize(20);
    QHeaderView *h_headerView = tbw_info->horizontalHeader();
    h_headerView->setStretchLastSection(true);                          //列最后的空间补全
    h_headerView->setDefaultAlignment(Qt::AlignHCenter);

    //Parameter Widget
    tbw_parameters = ui->table_parameters;
    tbw_parameters->setColumnCount(2);
    tbw_parameters->setColumnWidth(0, 150);
    tbw_parameters->setHorizontalHeaderLabels(QStringList()<<"Parameter"<<"Value");
    tbw_parameters->viewport()->setFocusPolicy(Qt::NoFocus);
    tbw_parameters->setSelectionBehavior(QAbstractItemView::SelectRows);
    tbw_parameters->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tbw_parameters->setStyleSheet("selection-background-color:lightblue;");
    tbw_parameters->setAlternatingRowColors(true);
    v_headerView = tbw_parameters->verticalHeader();
    v_headerView->setHidden(true);
    v_headerView->setDefaultSectionSize(20);
    h_headerView = tbw_parameters->horizontalHeader();
    h_headerView->setStretchLastSection(true);
    h_headerView->setDefaultAlignment(Qt::AlignHCenter);

    //Info Mul Widget
    tbw_info_mul = ui->table_info_mul;
    tbw_info_mul->setColumnCount(2);
    tbw_info_mul->setColumnWidth(0, 300);
    tbw_info_mul->setHorizontalHeaderLabels(QStringList()<<"Property"<<"Value");
    tbw_info_mul->viewport()->setFocusPolicy(Qt::NoFocus);
    tbw_info_mul->setSelectionBehavior(QAbstractItemView::SelectRows);
    tbw_info_mul->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tbw_info_mul->setStyleSheet("selection-background-color:lightblue;");
    v_headerView = tbw_info_mul->verticalHeader();
    v_headerView->setHidden(true);
    v_headerView->setDefaultSectionSize(20);
    h_headerView = tbw_info_mul->horizontalHeader();
    h_headerView->setStretchLastSection(true);
    h_headerView->setDefaultAlignment(Qt::AlignHCenter);
}
