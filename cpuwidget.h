#ifndef CPUWIDGET_H
#define CPUWIDGET_H
#include <QWidget>
#include <QPainter>
class CPUWidget : public QWidget{
    Q_OBJECT
public:
    CPUWidget(QWidget *parent = 0);
    ~CPUWidget();
protected:
    void paintEvent(QPaintEvent *);
};
#endif // CPUWIDGET_H
