#include "memwidget.h"

extern QVector<double> vcpu,vmem;
MemWidget::MemWidget(QWidget *parent): QWidget(parent){}
MemWidget::~MemWidget(){}
void MemWidget::paintEvent(QPaintEvent *){
    QPainter p(this);
    QPen pen = p.pen();
    pen.setColor(Qt::black);
    pen.setWidth(2);
    p.setPen(pen);
    p.drawRect(0,0,450,100);
    pen.setWidth(1);
    pen.setStyle(Qt::DotLine);
    p.setPen(pen);
    p.drawLine(0,25,450,25);
    p.drawLine(0,50,450,50);
    p.drawLine(0,75,450,75);
    pen.setStyle(Qt::SolidLine);
    pen.setColor(Qt::green);
    pen.setWidth(2);
    p.setPen(pen);
    //p.drawLine(0,0,450,100);
    while (vmem.size()>46) vmem.pop_front();
    int cx = 45;
    for (int i=vmem.size()-1;i>0;i--){
        p.drawLine(cx*10,100-(int)(vmem[i]*100),cx*10-10,100-(int)(vmem[i-1]*100));
        cx--;
    }
}
