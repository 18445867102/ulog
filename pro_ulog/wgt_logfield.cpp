#include "wgt_logfield.h"

FieldlistWidget::FieldlistWidget(QWidget *parent) : QWidget(parent)
{
    this->setWindowModality(Qt::ApplicationModal);
    this->setWindowTitle("Field List");
    this->setWindowIcon(QPixmap(":/images/app.ico"));
    int height = QApplication::desktop()->height();
    if(height < 800){
        height = QApplication::desktop()->height()*0.9;
        this->move ((QApplication::desktop()->width() - 500)/2,
                    (QApplication::desktop()->height())*0.04/2);
    }else{
        height = 800;
    }
    this->resize(500,height);
    //this->setAttribute(Qt::WA_DeleteOnClose);//窗体关闭释放内存

    btn_add = new QPushButton(this);
    btn_add->setText("Add");
    btn_close = new QPushButton(this);
    btn_close->setText("Close");
    lab_search = new QLabel(this);
    lab_search->setText(" Search: ");
    tex_search = new QTextEdit(this);
    tex_search->setMaximumHeight(25);

    QHBoxLayout *h_boxlayout = new QHBoxLayout;
    h_boxlayout->addWidget(lab_search);
    h_boxlayout->addWidget(tex_search);
    h_boxlayout->addWidget(btn_add);
    h_boxlayout->addWidget(btn_close);

    tbw_field = new QTableWidget(this);
    tbw_field->setColumnCount(2);
    tbw_field->setColumnWidth(0, 370);
    tbw_field->setHorizontalHeaderLabels(QStringList()<<"Field"<<"Type");
    tbw_field->setSelectionBehavior(QAbstractItemView::SelectRows);     //设置每次选择一行
    tbw_field->setEditTriggers(QAbstractItemView::NoEditTriggers);      //设置不可编辑
    tbw_field->viewport()->setFocusPolicy(Qt::NoFocus);
    tbw_field->setStyleSheet("selection-background-color:lightblue;");  //设置选中背景色
    tbw_field->setAlternatingRowColors(true);
    QHeaderView *v_headerView = tbw_field->verticalHeader();
    v_headerView->setHidden(true);
    v_headerView->setDefaultSectionSize(20);
    QHeaderView *h_headerView = tbw_field->horizontalHeader();
    h_headerView->setStretchLastSection(true);                          //列最后的空间补全
    h_headerView->setDefaultAlignment(Qt::AlignHCenter);

    QVBoxLayout *all_vboxlayout = new QVBoxLayout;
    all_vboxlayout->addWidget(tbw_field);
    all_vboxlayout->addLayout(h_boxlayout);
    all_vboxlayout->setContentsMargins(2,2,2,2);
    this->setLayout(all_vboxlayout);

    connect(btn_close,&QPushButton::clicked,
            [=]()
            {
                this->close();
            });
    connect(btn_add,&QPushButton::clicked,
            [=]()
            {
                emit signal_add_field();
            });
    connect(tex_search,&QTextEdit::textChanged,
            [=]()
            {
                emit signal_textchange(tex_search->toPlainText());
            });
    connect(tbw_field,SIGNAL(itemDoubleClicked(QTableWidgetItem *)),this,SLOT(solt_emitSignal(QTableWidgetItem *)));
}
void FieldlistWidget::solt_emitSignal(QTableWidgetItem* item){
     emit signal_itemDoubleClicked(item);
}
