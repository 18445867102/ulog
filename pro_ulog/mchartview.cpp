#include "mchartview.h"

MChartView::MChartView(QWidget *parent):QChartView(parent)
{
    this->setDragMode(QGraphicsView::RubberBandDrag);
    this->setMouseTracking(true);
    this->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    this->setResizeAnchor(QGraphicsView::AnchorUnderMouse);
//    this->setRubberBand(QChartView::RectangleRubberBand);//设置为矩形选择方式
//    this->setRubberBand(QChartView::VerticalRubberBand);
//    this->setRubberBand(QChartView::HorizontalRubberBand);
}
void MChartView::mousePressEvent(QMouseEvent *event){
    if (event->button()==Qt::LeftButton)
        beginPoint=event->pos();
    QChartView::mousePressEvent(event);
}

void MChartView::mouseMoveEvent(QMouseEvent *event){
    QPoint  point;
    point=event->pos();

    emit signal_mouseMove(point);
    QChartView::mouseMoveEvent(event);
}

void MChartView::mouseReleaseEvent(QMouseEvent *event){
    if (event->button()==Qt::LeftButton){
        endPoint=event->pos();
        QRectF  rectF;
        rectF.setTopLeft(beginPoint);
        rectF.setBottomRight(endPoint);

        emit signal_zoomSelect(rectF);
        chart()->zoomIn(rectF);
    }else if (event->button()==Qt::RightButton){
        emit signal_zoomReset();
        chart()->zoomReset();
    }
    QChartView::mouseReleaseEvent(event);
}
void MChartView::wheelEvent(QWheelEvent *event){
    if(event->delta() > 0){

        chart()->zoom(1.2);
    }else{
        emit signal_zoomWheel();
        chart()->zoom(0.8);
    }
}
void MChartView::resetChart(){
    emit signal_zoomReset();
    chart()->zoomReset();
}
void MChartView::leftShift(){
    emit signal_leftShift();
}
void MChartView::keyPressEvent(QKeyEvent *event){
    switch (event->key()) {
    case Qt::Key_Plus:
        chart()->zoom(1.2);
        break;
    case Qt::Key_Minus:
        chart()->zoom(0.8);
        break;
    case Qt::Key_Left:
        chart()->scroll(10, 0);
        break;
    case Qt::Key_Right:
        chart()->scroll(-10, 0);
        break;
    case Qt::Key_Up:
        chart()->scroll(0, -10);
        break;
    case Qt::Key_Down:
        chart()->scroll(0, 10);
        break;
    case Qt::Key_PageUp:
        chart()->scroll(0, 50);
        break;
    case Qt::Key_PageDown:
        chart()->scroll(0, -50);
        break;
    case Qt::Key_Home:
    //  chart()->zoomReset();
    //  chart()->resetTransform();//没用
    //  chart()->zoomIn(chart()->plotArea()); //没用
        break;
    default:
        break;
    }
    QGraphicsView::keyPressEvent(event);
}

MChartView::~MChartView(){

}
QColor MChartView::getColor(){
    static uint8_t index = 0;
    QColor color;

    switch (index) {
    case 0:
        color = QColor(245, 165, 20);
        break;
    case 1:
        color = QColor(85, 85, 225);
        break;
    case 2:
        color = Qt::darkMagenta;
        break;
    case 3:
        color = QColor(70, 170, 0);
        break;
    case 4:
        color = QColor(170, 0, 127);
        break;
    case 5:
        color = QColor(20, 170, 255);
        break;
    case 6:
        color = Qt::darkGreen;
        break;
    case 7:
        color = Qt::darkRed;
        break;
    default:
        break;
    }
    index++;
    index%=7;
    return color;
}
