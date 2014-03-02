#include "mainwindow.h"
#include <QApplication>
QVector<double> vcpu,vmem;
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    
    return a.exec();
}
