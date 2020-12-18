#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent),ui(new Ui_MainWindow)
{
    ui->setupUi(this);
    ui->splitter_v->setStretchFactor(0,8);
    ui->splitter_v->setStretchFactor(1,3);
    ui->splitter_h->setStretchFactor(0,1);
    ui->splitter_h->setStretchFactor(1,7);
    ui->horizontalSlider->setMinimum(12);
    ui->horizontalSlider->setMaximum(30);
    ui->horizontalSlider->setSingleStep(1);
    ui->horizontalSlider->setTracking(true);
    ui->horizontalSlider->setPageStep(2);

    this->setWindowTitle(tr("Ulog Analysis v1.1"));
    this->setWindowIcon(QPixmap(":/images/app.ico"));
    this->resize(QApplication::desktop()->width()*0.75,QApplication::desktop()->height()*0.8);
    this->setMinimumSize(800,500);

    //创建工具栏与初始化绘图窗口
    create_toolbar();
    create_chart();
    //创建子窗口对象
    fieldlist_widget = new FieldlistWidget;
    loginfo_widget = new LogInfoWidget;
    loggings_widget = new LoggingsWidget;
    //初始化底部状态栏
    stb_msg_lable = new QLabel("日志分析工具");
    stb_msg_lable->setMinimumSize(stb_msg_lable->sizeHint());
    stb_msg_lable->setAlignment(Qt::AlignHCenter);
    this->statusBar()->addWidget(stb_msg_lable);
    stb_point_lab = new QLabel("");
    stb_point_lab->setMinimumSize(stb_msg_lable->sizeHint());
    stb_point_lab->setAlignment(Qt::AlignHCenter);
    this->statusBar()->addPermanentWidget(stb_point_lab);
    //设置底部状态栏显示时间类型
    time.setTimeSpec(Qt::LocalTime);

    //设置主界面左上窗口
    tbw_plot = ui->tableWidget_plot;
    tbw_plot->setColumnCount(1);
    tbw_plot->setSelectionBehavior(QAbstractItemView::SelectRows);     //设置每次选择一行
    tbw_plot->setEditTriggers(QAbstractItemView::NoEditTriggers);      //设置不可编辑
    tbw_plot->viewport()->setFocusPolicy(Qt::NoFocus);
    tbw_plot->setItemDelegate(new ItemDelegate);
    //tbw_plot->setAlternatingRowColors(true);
    //tbw_plot->setStyleSheet("selection-background-color:lightblue;");
    QHeaderView *v_headerView = tbw_plot->verticalHeader();
    v_headerView->setHidden(true);
    v_headerView->setDefaultSectionSize(22);
    QHeaderView *h_headerView = tbw_plot->horizontalHeader();
    h_headerView->setHidden(true);                                      //水平表头隐藏
    h_headerView->setStretchLastSection(true);                          //列最后的空间补全
    h_headerView->setDefaultAlignment(Qt::AlignLeft);

    //创建CSV文件对象
    csv = new ExportCSV();

    connect(fieldlist_widget,SIGNAL(signal_textchange(QString)),this,SLOT(slot_change_fieldlist(QString)));
    connect(fieldlist_widget,SIGNAL(signal_add_field()),this,SLOT(slot_add_field()));
    connect(fieldlist_widget,SIGNAL(signal_itemDoubleClicked(QTableWidgetItem*)),this,SLOT(solt_itemDoubleClicked(QTableWidgetItem*)));
    connect(ui->btn_removeall,SIGNAL(clicked()),this,SLOT(solt_btn_removeall()));
    connect(ui->btn_remove,SIGNAL(clicked()),this,SLOT(solt_btn_remove()));
    connect(ui->actionmode1,SIGNAL(triggered()),this,SLOT(solt_export_csv_file1()));
    connect(ui->actionmode2,SIGNAL(triggered()),this,SLOT(solt_export_csv_file2()));
    connect(ui->actionBoot_Time,SIGNAL(toggled(bool)),this,SLOT(solt_setBootTime(bool)));
    connect(ui->actionUtc_Time,SIGNAL(toggled(bool)),this,SLOT(solt_setUtcTime(bool)));
    connect(ui->horizontalSlider,SIGNAL(sliderMoved(int)),this,SLOT(solt_sliderMoved(int)));
    connect(ui->horizontalSlider,SIGNAL(singal_sliderPressed(int)),this,SLOT(solt_sliderMoved(int)));
    connect(tbw_plot,SIGNAL(itemClicked(QTableWidgetItem *)),this,SLOT(solt_itemclick(QTableWidgetItem*)));
    connect(tbw_plot,SIGNAL(itemDoubleClicked(QTableWidgetItem *)),this,SLOT(solt_plotDoubleClicked(QTableWidgetItem*)));
    connect(ui->actionOpen_Log,SIGNAL(triggered()),this,SLOT(open_log()));
    connect(ui->exit,&QAction::triggered, [=](){ exit(0);});
    connect(ui->comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(solt_setAnimation(int)));
    connect(ui->comboBox_2,SIGNAL(currentIndexChanged(int)),this,SLOT(solt_setTheme(int)));

    connect(ui->btn_up,&QPushButton::pressed,[=](){chart->scroll(0,-(ui->doubleSpinBox->value()));});
    connect(ui->btn_down,&QPushButton::pressed,[=](){chart->scroll(0,ui->doubleSpinBox->value());});
    connect(ui->btn_left,&QPushButton::pressed,
            [=](){
                    chart->scroll(ui->doubleSpinBox->value(),0);
                    chart_view->leftShift();
            });
    connect(ui->btn_right,&QPushButton::pressed,
            [=](){
                    chart->scroll(-(ui->doubleSpinBox->value()),0);
                    chart_view->leftShift();
            });
    readSettings();
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::create_toolbar(){
    QToolBar *set_toolbar =addToolBar("Set");

    tba_btn_open_log = new QToolButton;
    tba_btn_open_log->setText(tr("打开日志"));
    tba_btn_open_log->setFont(QFont("微软雅黑",10));
    tba_btn_word_list = new QToolButton;
    tba_btn_word_list->setText(tr("字段列表"));
    tba_btn_word_list->setFont(QFont("微软雅黑",10));
    tba_btn_log_info = new QToolButton;
    tba_btn_log_info->setText(tr("日志摘要"));
    tba_btn_log_info->setFont(QFont("微软雅黑",10));
    tba_chk_data_mark = new QCheckBox;
    tba_chk_data_mark->setText(tr("数据标记"));
    tba_chk_data_mark->setFont(QFont("微软雅黑",10));
    tba_chk_data_mark->setCheckState(Qt::Unchecked);
    tba_btn_logmessage = new QToolButton;
    tba_btn_logmessage->setText(tr("日志信息"));
    tba_btn_logmessage->setFont(QFont("微软雅黑",10));
    tba_lab_myset = new QLabel(tr("  预设: "));
    tba_cob_setname = new QComboBox();
    tba_cob_setname->setMinimumWidth(150);
    tba_btn_save = new QToolButton;
    tba_btn_save->setText(tr("保存预设"));
    tba_btn_delete = new QToolButton;
    tba_btn_delete->setText(tr("删除预设"));

    ui->mainToolBar->addWidget(tba_btn_open_log);
    ui->mainToolBar->addWidget(tba_btn_word_list);
    ui->mainToolBar->addWidget(tba_btn_log_info);
    ui->mainToolBar->addWidget(tba_btn_logmessage);
    ui->mainToolBar->addSeparator();
    set_toolbar->addWidget(tba_chk_data_mark);

    ui->btn_up->setAutoRepeat(true);
    ui->btn_up->setAutoRepeatDelay(50);
    ui->btn_down->setAutoRepeat(true);
    ui->btn_down->setAutoRepeatDelay(50);
    ui->btn_left->setAutoRepeat(true);
    ui->btn_left->setAutoRepeatDelay(50);
    ui->btn_right->setAutoRepeat(true);
    ui->btn_right->setAutoRepeatDelay(50);

    connect(tba_btn_open_log,SIGNAL(clicked()),this,SLOT(open_log()));
    connect(tba_btn_word_list,SIGNAL(clicked()),this,SLOT(open_fieldlist_widget()));
    connect(ui->btn_addpro,SIGNAL(clicked()),this,SLOT(open_fieldlist_widget()));
    connect(tba_btn_logmessage,SIGNAL(clicked()),this,SLOT(open_logmessage_widget()));
    connect(tba_btn_log_info,SIGNAL(clicked()),this,SLOT(open_loginfo_widget()));
}

void MainWindow::open_log(){
    QString cur_path = QDir::currentPath();                  //获取应用程序当前路径
    QString dlg_title = "选择日志";                           //对话框标题"
    QString filter="Log Files(*.px4log, *.bin, *.ulg);;"     //文件过滤器
                   "PX4/APM Log(*.px4log, *.bin);;"
                   "Ulog(*.ulg)";
    QString FilePath=QFileDialog::getOpenFileName(this,dlg_title,cur_path,filter);

    if(FilePath.isNull())
        return;
    else
        stb_msg_lable->setText("当前日志: "+FilePath);

    csv->CSV_Set_Path(FilePath);

    if(mlog != nullptr){
        delete mlog;
        tbw_plot->setRowCount(0);
    }
    mlog = new ulog;
    connect(mlog,SIGNAL(finished()),this,SLOT(solt_ulog_finished()));

    mlog->ulog_open_file(FilePath.toLocal8Bit().toStdString());
    mlog->start();
}
//槽函数，显示字段列表子窗口
void MainWindow::open_fieldlist_widget(){
    loggings_widget->close();
    loginfo_widget->close();
    fieldlist_widget->show();
    fieldlist_widget->activateWindow();

    if(mlog == nullptr)
        return;

    fieldlist_widget->tbw_field->setRowCount(0);
    map<string,string>::iterator map_it=mlog->ulog_msgname_type.begin();
    while (map_it!=mlog->ulog_msgname_type.end()){
        int row_count = fieldlist_widget->tbw_field->rowCount();
        fieldlist_widget->tbw_field->insertRow(row_count);
        QTableWidgetItem *key = new QTableWidgetItem ();
        QTableWidgetItem *value = new QTableWidgetItem ();
        key->setText(map_it->first.c_str());
        value->setText(map_it++->second.c_str());
        fieldlist_widget->tbw_field->setItem(row_count, 0, key);
        fieldlist_widget->tbw_field->setItem(row_count, 1, value);
    }
}
//槽函数，显示日志信息子窗口
void MainWindow::open_loginfo_widget(){
    loginfo_widget->show();
    loggings_widget->close();
    loginfo_widget->activateWindow();

    if(mlog == nullptr)
        return;
    //填充数据
    map<string, string>::iterator iter = mlog->ulog_info_map.begin();
    loginfo_widget->tbw_info->setRowCount(0);
    while(iter != mlog->ulog_info_map.end()){
        int row_count = loginfo_widget->tbw_info->rowCount();
        loginfo_widget->tbw_info->insertRow(row_count);
        QTableWidgetItem *key = new QTableWidgetItem ();
        QTableWidgetItem *value = new QTableWidgetItem ();
        key->setText(iter->first.c_str());
        value->setText(iter->second.c_str());
        loginfo_widget->tbw_info->setItem(row_count, 0, key);
        loginfo_widget->tbw_info->setItem(row_count, 1, value);
        iter++;
    }

    iter = mlog->ulog_parameter_map.begin();
    loginfo_widget->tbw_parameters->setRowCount(0);
    while(iter != mlog->ulog_parameter_map.end()){
        int row_count = loginfo_widget->tbw_parameters->rowCount();
        loginfo_widget->tbw_parameters->insertRow(row_count);
        QTableWidgetItem *key = new QTableWidgetItem ();
        QTableWidgetItem *value = new QTableWidgetItem ();
        key->setText(iter->first.c_str());
        value->setText(iter->second.c_str());
        loginfo_widget->tbw_parameters->setItem(row_count, 0, key);
        loginfo_widget->tbw_parameters->setItem(row_count, 1, value);
        iter++;
    }

    iter = mlog->ulog_multi_info_map.begin();
    loginfo_widget->tbw_info_mul->setRowCount(0);
    while(iter != mlog->ulog_multi_info_map.end()){
        int row_count = loginfo_widget->tbw_info_mul->rowCount();
        loginfo_widget->tbw_info_mul->insertRow(row_count);
        QTableWidgetItem *key = new QTableWidgetItem ();
        QTableWidgetItem *value = new QTableWidgetItem ();
        key->setText(iter->first.c_str());
        value->setText(iter->second.c_str());
        loginfo_widget->tbw_info_mul->setItem(row_count, 0, key);
        loginfo_widget->tbw_info_mul->setItem(row_count, 1, value);
        iter++;
    }
}
//槽函数，根据搜索栏内容重新填充字段列表
void MainWindow::slot_change_fieldlist(QString str){
    if(mlog == nullptr)
        return;

    fieldlist_widget->tbw_field->setRowCount(0);
    QStringList key_filter = msgname_type.keys().filter(str);
    for(int i = 0; i< key_filter.size();++i){
        QString s = key_filter.at(i);
        int row_count = fieldlist_widget->tbw_field->rowCount();
        fieldlist_widget->tbw_field->insertRow(row_count);
        QTableWidgetItem *key = new QTableWidgetItem ();
        QTableWidgetItem *value = new QTableWidgetItem ();
        key->setText(s);
        value->setText(msgname_type.value(s));
        fieldlist_widget->tbw_field->setItem(row_count, 0, key);
        fieldlist_widget->tbw_field->setItem(row_count, 1, value);
    }
}
//槽函数，显示日志打印信息子窗口
void MainWindow::open_logmessage_widget(){
    loggings_widget->show();
    loginfo_widget->close();
    loggings_widget->activateWindow();

    if(mlog == nullptr)
        return;

    QDateTime t;
    loggings_widget->tbw_logMessage->setRowCount(0);
    multimap<uint64_t, string>::iterator iter = mlog->ulog_loggings.begin(); 
    while(iter != mlog->ulog_loggings.end()){
        int row_count = loggings_widget->tbw_logMessage->rowCount();
        loggings_widget->tbw_logMessage->insertRow(row_count);
        QTableWidgetItem *time = new QTableWidgetItem ();
        QTableWidgetItem *level = new QTableWidgetItem ();
        QTableWidgetItem *logs = new QTableWidgetItem ();
        t.setMSecsSinceEpoch(iter->first);
        time->setText(t.toString("yyyy-MM-dd  hh:mm:ss:zzz"));
        level->setText(iter++->second.c_str());
        logs->setText(iter++->second.c_str());
        loggings_widget->tbw_logMessage->setItem(row_count, 0, time);
        loggings_widget->tbw_logMessage->setItem(row_count, 1, level);
        loggings_widget->tbw_logMessage->setItem(row_count, 2, logs);
    }
}
//槽函数，字段列表窗口中添加按钮的槽函数
void MainWindow::slot_add_field(){
    uint32_t timeIndex,dataIndex;
    QString field_name,str;
    MItem *name = nullptr;
    int row_count;
    QList<QTableWidgetItem*> p_item = fieldlist_widget->tbw_field->selectedItems();
    QRegExp rx("\\.[^.*]+");

    chart_view->resetChart();
    for(int i =0; i<p_item.size(); i+=2){
        row_count = tbw_plot->rowCount();
        tbw_plot->insertRow(row_count);


        field_name = p_item[i]->text();
        str = field_name;
        rx.lastIndexIn(field_name);
        str.replace(rx.cap(),".timestamp");
        dataIndex = msgname_num.value(field_name);
        timeIndex = msgname_num.value(str);

        name = new MItem(chart_view,&mlog->ulog_data[timeIndex],&mlog->ulog_data[dataIndex],msgname_type.value(field_name), field_name);
        connect(tba_chk_data_mark,SIGNAL(toggled(bool)),name,SLOT(solt_pointsVisible(bool)));
        tbw_plot->setItem(row_count,0, name);
    }
    chart->createDefaultAxes();

    if(timeType == false) {
        timeAxisX = new QDateTimeAxis;
        timeAxisX->setFormat("mm:ss:zzz");
        timeAxisX->setTickCount(9);
        chart->setAxisX(timeAxisX,chart->series().at(0));
    }

    fieldlist_widget->close();
}
//双击字段添加到绘制窗口
void MainWindow::solt_itemDoubleClicked(QTableWidgetItem* item){
    uint32_t timeIndex,dataIndex;
    QString str, field_name;
    QTableWidgetItem *selectItem;
    int row_count;
    QRegExp rx("\\.[^.*]+");

    selectItem = item->tableWidget()->item(item->row(),0);
    row_count = tbw_plot->rowCount();
    tbw_plot->insertRow(row_count);

    field_name = selectItem->text();
    str = field_name;
    rx.lastIndexIn(field_name);
    str.replace(rx.cap(),".timestamp");
    dataIndex = msgname_num.value(field_name);
    timeIndex = msgname_num.value(str);

    chart_view->resetChart();
    MItem *name = new MItem(chart_view,&mlog->ulog_data[timeIndex],&mlog->ulog_data[dataIndex],msgname_type.value(field_name), field_name);
    connect(tba_chk_data_mark,SIGNAL(toggled(bool)),name,SLOT(solt_pointsVisible(bool)));
    chart->createDefaultAxes();

    if(timeType == false) {
        timeAxisX = new QDateTimeAxis;
        timeAxisX->setFormat("mm:ss:zzz");
        timeAxisX->setTickCount(9);
        chart->setAxisX(timeAxisX,chart->series().at(0));
    }
    tbw_plot->setItem(row_count,0, name);
}
//槽函数，导出CSV文件
void MainWindow::solt_export_csv_file1()
{
   csv->CSV_Get_Data(mlog,1);
}
void MainWindow::solt_export_csv_file2()
{
   csv->CSV_Get_Data(mlog,2);
}
//创建图表并初始化
void MainWindow::create_chart(){
    chart = new QChart();
    chart->setMargins(QMargins(10,10,10,10));
    chart->layout()->setContentsMargins(2,4,4,2);
    chart->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    chart->legend()->setVisible(false);
    chart_view = new MChartView(this);
    chart_view->setChart(chart);
    chart_view->setRenderHint(QPainter::Antialiasing);
    chart_view->setCursor(Qt::CrossCursor);
    //窗口布局
    QVBoxLayout *vboxlayout = new QVBoxLayout;
    vboxlayout->addWidget(chart_view);
    vboxlayout->setContentsMargins(0,2,2,2);
    ui->widget->setLayout(vboxlayout);

    //创建初始的坐标轴
    axisY = new QValueAxis;
    axisY->setRange(-1, 1);
    axisY->setTickCount(11);
    timeAxisX = new QDateTimeAxis;
    timeAxisX->setFormat("mm:ss:zzz");
    timeAxisX->setTickCount(9);
    timeAxisX->setRange(QDateTime(QDate(1970,1,1)),QDateTime(QDate(2019,1,2)));

    chart->setAxisX(timeAxisX);
    chart->setAxisY(axisY);

    connect(chart_view,SIGNAL(signal_mouseMove(QPoint)),this, SLOT(solt_mouseMove(QPoint)));
}
//显示鼠标指针对应图标的坐标
void MainWindow::solt_mouseMove(QPoint point){
    //转换为图表的数值
    QPointF pt=chart->mapToValue(point);
    time.setMSecsSinceEpoch(static_cast<int64_t>(pt.x()));
    //底部状态栏显示日期时间和数值
    stb_point_lab->setText("Date: "+time.toString("yyyy-MM-dd hh:mm:ss   ")+QString::asprintf("Value: %.3f ",pt.y()));
}
void MainWindow::solt_btn_removeall(){
    tbw_plot->setRowCount(0);
    ui->horizontalSlider->setValue(0);
}
//左上窗口移除曲线按钮槽函数
void MainWindow::solt_btn_remove(){
    QList<QTableWidgetItem*> p_item = tbw_plot->selectedItems();
    for(int i =0; i<p_item.size(); i++)
        tbw_plot->removeRow(p_item[i]->row());
    ui->horizontalSlider->setValue(0);
}
//槽函数，日志解析完成后的数据处理
void MainWindow::solt_ulog_finished(){
    map<string, string>::iterator map_iter_str_str = mlog->ulog_msgname_type.begin();
    while(map_iter_str_str != mlog->ulog_msgname_type.end()){
        msgname_type.insert(map_iter_str_str->first.c_str(),map_iter_str_str->second.c_str());
        map_iter_str_str++;
    }
    map<string, uint16_t>::iterator map_iter_str_u16 =mlog->ulog_msgname_num.begin();
    while(map_iter_str_u16 != mlog->ulog_msgname_num.end()){
        msgname_num.insert(map_iter_str_u16->first.c_str(),map_iter_str_u16->second);
        map_iter_str_u16++;
    }
}

void MainWindow::solt_itemclick(QTableWidgetItem *item){
    MItem* m = static_cast<MItem*>(item);
    qreal alpha;
    QColor color;

    ui->horizontalSlider->setValue(static_cast<int>(m->penWidthF * 10));
    if(m->checkState() == Qt::Checked){
        alpha = 1.0;
        if(m->m_series!=nullptr)
            m->m_series->setVisible(true);
    }else{
        alpha = 0.3;
        if(m->m_series!=nullptr)
            m->m_series->setVisible(false);
    }
    color = m->color;
    color.setAlphaF(alpha);
    m->setForeground(color);
}
void MainWindow::solt_plotDoubleClicked(QTableWidgetItem*item){
    MItem* m = static_cast<MItem*>(item);
    QColor color = QColorDialog::getColor();
    if(!color.isValid())
        return;
    m->color = color;
    m->pen.setColor(color);
    m->m_series->setPen(m->pen);
    m->setForeground(color);
}
void MainWindow::solt_sliderMoved(int value){
    MItem* item = static_cast<MItem*>(tbw_plot->currentItem());
    if(item == nullptr)
        return;
    item->penWidthF = value/10.0;
    item->pen.setWidthF(value/10.0);
    item->m_series->setPen(item->pen);
}
void MainWindow::solt_setUtcTime(bool checked){
    if(checked == true){
        timeType = false;
        timeAxisX = new QDateTimeAxis;
        timeAxisX->setFormat("mm:ss:zzz");
        timeAxisX->setTickCount(9);
        chart->setAxisX(timeAxisX,chart->series().at(0));
        if(ui->actionBoot_Time->isChecked())
            ui->actionBoot_Time->setChecked(false);
    }else{
        if(!ui->actionBoot_Time->isChecked())
            ui->actionUtc_Time->setChecked(true);
    }
}
void MainWindow::solt_setBootTime(bool checked){
    if(checked == true){
        timeType = true;
        chart->createDefaultAxes();
        if(ui->actionUtc_Time->isChecked())
            ui->actionUtc_Time->setChecked(false);
    }else{
        if(!ui->actionUtc_Time->isChecked())
            ui->actionBoot_Time->setChecked(true);
    }
}
void MainWindow::solt_setAnimation(int index){
    switch (index) {
    case 0:
        chart->setAnimationOptions(QChart::NoAnimation);
        break;
    case 1:
        chart->setAnimationOptions(QChart::SeriesAnimations);
        break;
    case 2:
        chart->setAnimationOptions(QChart::GridAxisAnimations);
        break;
    case 3:
        chart->setAnimationOptions(QChart::AllAnimations);
        break;
    default:
        break;
    }
}
void MainWindow::solt_setTheme(int index){
    switch (index) {
    case 0:
        chart->setTheme(QChart::ChartThemeLight);
        break;
    case 1:
        chart->setTheme(QChart::ChartThemeBlueCerulean);
        break;
    case 2:
        chart->setTheme(QChart::ChartThemeDark);
        break;
    case 3:
        chart->setTheme(QChart::ChartThemeBrownSand);
        break;
    case 4:
        chart->setTheme(QChart::ChartThemeBlueNcs);
        break;
    case 5:
        chart->setTheme(QChart::ChartThemeHighContrast);
        break;
    case 6:
        chart->setTheme(QChart::ChartThemeBlueIcy);
        break;
    case 7:
        chart->setTheme(QChart::ChartThemeQt);
        break;
    default:
        break;
    }
}

void MainWindow::writeSettings()
{
    QSettings set("MyApp", "setting");
    set.setValue("theme", ui->comboBox_2->currentIndex());
    set.setValue("animation", ui->comboBox->currentIndex());
}

void MainWindow::readSettings()
{
    QSettings set("MyApp", "setting");
    ui->comboBox->setCurrentIndex(set.value("animation").toInt());
    ui->comboBox_2->setCurrentIndex(set.value("theme").toInt());
}
void MainWindow::changeEvent(QEvent *event){
    if(event->type() == QEvent::ActivationChange){
        if(this->isActiveWindow()){
            loggings_widget->close();
            loginfo_widget->close();
        }
    }
    QMainWindow::changeEvent(event);
}
void MainWindow::closeEvent(QCloseEvent *event){
    Q_UNUSED(event);
    writeSettings();
    tbw_plot->setRowCount(0);
    fieldlist_widget->deleteLater();
    loginfo_widget->deleteLater();
    loggings_widget->deleteLater();
    if(mlog != nullptr)
        delete mlog;
    if(csv != nullptr)
        delete csv;
}
