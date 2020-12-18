#ifndef WGT_FIELDLIST_H
#define WGT_FIELDLIST_H

#include <QApplication>
#include <QDesktopWidget>
#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QHeaderView>

class FieldlistWidget : public QWidget
{
    Q_OBJECT
public:
    FieldlistWidget(QWidget *parent = nullptr);
    QTableWidget *tbw_field;
signals:
    void signal_textchange(QString);
    void signal_add_field();
    void signal_itemDoubleClicked(QTableWidgetItem*);
public slots:
    void solt_emitSignal(QTableWidgetItem*);
private:
    QPushButton *btn_add;
    QPushButton *btn_close;
    QLabel *lab_search;
    QTextEdit *tex_search;
};

#endif
//WGT_FIELDLIST_H
