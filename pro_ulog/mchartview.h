#ifndef MCHARTVIEW_H
#define MCHARTVIEW_H

#include <QChartView>
#include <QtCharts>
#include <QPoint>
#include <QDebug>
#include <QObject>
QT_CHARTS_USE_NAMESPACE

class MChartView : public QChartView
{
    Q_OBJECT

private:
    QPoint  beginPoint; //选择矩形区的起点
    QPoint  endPoint;   //选择矩形区的终点
protected:
   // bool viewportEvent(QEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);
signals:
    void signal_mouseMove(QPoint);
    void signal_zoomSelect(QRectF);
    void signal_zoomWheel();
    void signal_zoomReset();
    void signal_leftShift();
public:
    MChartView(QWidget *parent = nullptr);
    ~MChartView();

    QColor getColor();
    void resetChart();
    void leftShift();
};

#endif // MCHARTVIEW_H
