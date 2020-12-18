#include "mitem.h"
using namespace QtCharts;

MItem::MItem(MChartView *view, QByteArray *t,QByteArray *d, QString type, QString fieldname,QObject *parent)
    :QObject(parent),color(view->getColor())
{
    this->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled|Qt::ItemIsSelectable);
    this->setCheckState(Qt::Checked);
    this->setForeground(color);
    this->setText(fieldname);

    if(t->size()==0 || d->size()==0)
        return;

    penWidthF = 1.2;
    pen.setColor(color);
    pen.setWidthF(penWidthF);

    QFont font = this->font();
    font.setBold(true);
    this->setFont(font);

    m_series = new QLineSeries;
    m_series->setPen(pen);
    m_view = view;
    begin = 0;
    end = count;

    uint32_t d_size = static_cast<uint32_t>(d->size());
    if(!QString::compare("float", type)){
        count= d_size>>2;
        data = new double[count];
        float *temp =new float[count];
        memcpy(temp, d->data(), d_size);
        for(uint64_t i=0; i<count; i++)
            data[i] = temp[i];
        delete []temp;
    }else if(!QString::compare("uint8_t", type)){
        count= d_size;
        data = new double[count];
        uint8_t *temp =new uint8_t[count];
        memcpy(temp, d->data(), d_size);
        for(uint64_t i=0; i<count; i++)
            data[i] = temp[i];
        delete []temp;
    }else if(!QString::compare("uint32_t",type)){
        count= d_size>>2;
        data = new double[count];
        uint32_t *temp =new uint32_t[count];
        memcpy(temp, d->data(), d_size);
        for(uint64_t i=0; i<count; i++)
            data[i] = temp[i];
        delete []temp;
    }else if(!QString::compare("uint64_t", type)){
        count= d_size>>3;
        data = new double[count];
        uint64_t *temp =new uint64_t[count];
        memcpy(temp, d->data(), d_size);
        for(uint64_t i=0; i<count; i++)
            data[i] = temp[i]/1000;
        delete []temp;
    }else if(!QString::compare("double",type)){
        count= d_size>>3;
        data = new double[count];
        memcpy(data, d->data(), d_size);
    }else if(!QString::compare("int32_t", type)){
        count= d_size>>2;
        data = new double[count];
        int32_t *temp =new int32_t[count];
        memcpy(temp, d->data(), d_size);
        for(uint64_t i=0; i<count; i++)
            data[i] = temp[i];
    }else if(!QString::compare("int64_t", type)){
        count= d_size>>3;
        data = new double[count];
        int64_t *temp =new int64_t[count];
        memcpy(temp, d->data(), d_size);
        for(uint64_t i=0; i<count; i++)
            data[i] = temp[i];
        delete []temp;
    }else if(!QString::compare("int8_t", type)){
        count= d_size;
        data = new double[count];
        int8_t *temp =new int8_t[count];
        memcpy(temp, d->data(), d_size);
        for(uint64_t i=0; i<count; i++)
            data[i] = temp[i];
        delete []temp;
    }else if(!QString::compare("char", type)){
        count= d_size;
        data = new double[count];
        char *temp =new char[count];
        memcpy(temp, d->data(), d_size);
        for(uint64_t i=0; i<count; i++)
            data[i] = temp[i];
        delete []temp;
    }else if(!QString::compare("bool", type)){
        count= d_size;
        data = new double[count];
        uint8_t *temp =new uint8_t[count];
        memcpy(temp, d->data(), d_size);
        for(uint64_t i=0; i<count; i++)
            data[i] = temp[i];
    }else if(!QString::compare("uint16_t", type)){
        count= d_size>>1;
        data = new double[count];
        uint16_t *temp =new uint16_t[count];
        memcpy(temp, d->data(), d_size);
        for(uint64_t i=0; i<count; i++)
            data[i] = temp[i];
        delete []temp;
    }else if(!QString::compare("int16_t", type)){
        count= d_size>>1;
        data = new double[count];
        int16_t *temp =new int16_t[count];
        memcpy(temp, d->data(), d_size);
        for(uint64_t i=0; i<count; i++)
            data[i] = temp[i];
        delete []temp;
    }else
        qDebug()<<"error.";

    time = new uint64_t[count];
    memcpy(time,t->data(), static_cast<uint32_t>(t->size()));
    for(uint64_t i=0; i<count; i++)
        time[i] /= 1000;

    if(count>MAX_POINTS)
        skip = count>>11;
    else
        skip = 1;

    for(uint64_t i=0; i<count-skip; i+=skip)
        m_series->append(time[i],data[i]);

    view->chart()->addSeries(m_series);
    connect(view,SIGNAL(signal_zoomSelect(QRectF)),this,SLOT(slot_zoomSelect(QRectF)));
    connect(view,SIGNAL(signal_zoomWheel()),this,SLOT(slot_zoomWheel()));
    connect(view,SIGNAL(signal_zoomReset()),this,SLOT(slot_zoomReset()));
    connect(view,SIGNAL(signal_leftShift()),this,SLOT(slot_leftShift()));
}
void MItem::slot_zoomWheel(){
    uint32_t i;

    x1 = m_view->chart()->mapToValue(m_view->chart()->plotArea().topLeft()).x();
    x2 = m_view->chart()->mapToValue(m_view->chart()->plotArea().bottomRight()).x();

    if((x1 > time[0]) && (x2 < time[count-1])){
        for(i=0; time[i]<x1; i+=skip);
        begin = i;
        for(;time[i]<x2; i+=skip);
        end = i;
        if(begin < (end-begin)*0.1)
            begin = 0;
        else
            begin = static_cast<uint32_t>(1.1*begin - 0.1*end);
        if((1.1*end - 0.1*begin) > count)
            end = count;
        else
            end = static_cast<uint32_t>(1.1*end - 0.1*begin);
    }else if((x1 < time[0]) && (x2 < time[count-1])){
        if(x2<time[0]){
            return;
        }else{
            begin = 0;
            for(i=0; time[i]<x2; i+=skip);
            end = i;
            if((1.2*end - 0.2*begin) > count)
                end = count;
            else
                end = static_cast<uint32_t>(1.2*end - 0.2*begin);
        }
    }else if((x1 > time[0]) && (x2 > time[count-1])){
        if(x1>time[count-1]){
            return;
        }else{
            end = count;
            for(i=0; time[i]<x1; i+=skip);
            begin = i;
            if((begin < (end-begin)*0.2))
                begin = 0;
            else
                begin = static_cast<uint32_t>(1.2*begin - 0.2*end);
        }
    }else{
        begin = 0;
        end = count;
    }

    if((end-begin)>MAX_POINTS)
        skip = (end-begin)>>11;
    else
        skip = 1;

    m_buffer.clear();
    for(i=begin; i<end; i+=skip)
        m_buffer.append(QPointF(time[i],data[i]));
    m_series->replace(m_buffer);
}
void MItem::slot_zoomSelect(QRectF rect){
    uint32_t i;

    x1 = m_view->chart()->mapToValue(rect.topLeft()).x();
    x2 = m_view->chart()->mapToValue(rect.bottomRight()).x();

    if((x1>time[0]) && (x2<time[count-1])){
        for(i=begin; time[i]<x1; i+=skip);
        begin = i;
        for(;time[i]<x2; i+=skip);
        end = i;
    }else if((x1<time[0]) && (x2<time[count-1])){
        if(x2<time[0]){
            return;
        }else{
            begin = 0;
            for(i = 0; time[i]<x2; i+=skip);
            end = i;
        }
    }else if((x1>time[0]) && (x2>time[count-1])){
        if(x1>time[count-1]){
            return;
        }else{           
            for(i = begin; time[i]<x1; i+=skip);
            begin = i;
            end = count;
        }
    }else{
        begin = 0;
        end = count;
    }

    if((end-begin)>MAX_POINTS)
        skip = (end-begin)>>11;
    else{
        if(skip == 1)
            return;
        skip = 1;
    }

    m_buffer.clear();
    for(i=begin; i<end; i+=skip)
        m_buffer.append(QPointF(time[i],data[i]));
    m_series->replace(m_buffer);
}
void MItem::slot_zoomReset(){
    begin = 0;
    end = count;
    if(count>MAX_POINTS)
        skip = count>>11;
    else{
        if(skip == 1)
            return;
        skip = 1;
    }

    m_buffer.clear();
    for(uint64_t i=0; i<count; i+=skip)
        m_buffer.append(QPointF(time[i],data[i]));
    m_series->replace(m_buffer);
}
void MItem::slot_leftShift(){
    uint32_t i;

    x1 = m_view->chart()->mapToValue(m_view->chart()->plotArea().topLeft()).x();
    x2 = m_view->chart()->mapToValue(m_view->chart()->plotArea().bottomRight()).x();

    if((x1 > time[0]) && (x2 < time[count-1])){
        for(i=0; time[i]<x1; i+=skip);
        begin = i;
        for(;time[i]<x2; i+=skip);
        end = i;

    }else if((x1 < time[0]) && (x2 < time[count-1])){
        if(x2<time[0]){
            return;
        }else{
            begin = 0;
            for(i=0; time[i]<x2; i+=skip);
            end = i;
        }
    }else if((x1 > time[0]) && (x2 > time[count-1])){
        if(x1>time[count-1]){
            return;
        }else{
            end = count;
            for(i=0; time[i]<x1; i+=skip);
            begin = i;
        }
    }else{
        begin = 0;
        end = count;
    }

    m_buffer.clear();
    for(i=begin; i<end; i+=skip)
        m_buffer.append(QPointF(time[i],data[i]));
    m_series->replace(m_buffer);
}

//槽函数，设置数据点是否可见
void MItem::solt_pointsVisible(bool state){
    if(m_series != nullptr){
        if(state == true)
            m_series->setPointsVisible(true);
        else
            m_series->setPointsVisible(false);
    }
}
//析构函数
MItem::~MItem(){
    if(m_series != nullptr){
        delete []time;
        delete []data;
        m_view->chart()->removeSeries(m_series);
        m_view->chart()->createDefaultAxes();
    }
}
