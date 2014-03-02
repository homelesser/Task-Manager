#ifndef MEMWIDGET_H
#define MEMWIDGET_H
#include <QWidget>
#include <QPainter>
class MemWidget : public QWidget{
    Q_OBJECT
public:
    MemWidget(QWidget *parent = 0);
    ~MemWidget();
protected:
    void paintEvent(QPaintEvent *);
};
#endif // MEMWIDGET_H
