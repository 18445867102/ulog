#ifndef MSLIDER_H
#define MSLIDER_H

#include <QSlider>
#include <QMouseEvent>

class MSlider:public QSlider
{
    Q_OBJECT
public:
    MSlider(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *);

signals:
    void singal_sliderPressed(int);
};

#endif // MSLIDER_H
