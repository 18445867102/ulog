#ifndef MTEMPLATE_H
#define MTEMPLATE_H

#include <QChartView>
#include <QtCharts>
#include <QObject>
#include <QLineSeries>
#include "ulog.h"
#include "mchartview.h"

using namespace QtCharts;
#define MAX_POINTS  2048

class MItem: public QObject, public QTableWidgetItem
{
    Q_OBJECT
public:
    explicit MItem(MChartView *view, QByteArray *t,QByteArray *d,QString type, QString fieldname, QObject *parent = nullptr);
    ~MItem();

    QPen pen;
    QColor color;
    qreal penWidthF;
    QChartView* m_view;
    QLineSeries* m_series = nullptr;
    QVector<QPointF> m_buffer;

    double *data;
    uint64_t *time;
    uint32_t count;
    uint32_t skip;
    uint32_t begin,end;
    double x1,x2;
public slots:
    void slot_zoomWheel();
    void slot_zoomSelect(QRectF);
    void slot_leftShift();
    void slot_zoomReset();
    void solt_pointsVisible(bool);
};

#endif // MTEMPLATE_H
