#include "cpuwidget.h"
extern QVector<double> vcpu;
CPUWidget::CPUWidget(QWidget *parent): QWidget(parent){}
CPUWidget::~CPUWidget(){}
void CPUWidget::paintEvent(QPaintEvent *){
    QPainter p(this);
    //p.drawLine(0,0,450,100);
    //Qt::SolidLine,Qt::DashLine,Qt::DotLine,Qt::DashDotLine,Qt::DashDotDotLine
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
    pen.setColor(Qt::red);
    pen.setWidth(2);
    p.setPen(pen);
    while (vcpu.size()>46) vcpu.pop_front();
    int cx = 45;
    for (int i=vcpu.size()-1;i>0;i--){
        p.drawLine(cx*10,100-(int)(vcpu[i]*100),cx*10-10,100-(int)(vcpu[i-1]*100));
        cx--;
    }

}
