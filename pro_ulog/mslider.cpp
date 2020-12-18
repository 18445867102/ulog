#include "mslider.h"

MSlider::MSlider(QWidget *parent):QSlider(parent)
{

}
void MSlider::mousePressEvent(QMouseEvent *ev){
    int value;
    //注意应先调用父类的鼠标点击处理事件，这样可以不影响拖动的情况
    QSlider::mousePressEvent(ev);
    //获取鼠标的位置，这里并不能直接从ev中取值（因为如果是拖动的话，鼠标开始点击的位置没有意义了）
    double pos = ev->pos().x() / static_cast<double>(width());
    value = static_cast<int>(pos * (maximum() - minimum())) + minimum();
    setValue(value);
    emit singal_sliderPressed(value);
}
