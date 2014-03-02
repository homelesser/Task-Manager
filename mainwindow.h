#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <cpuwidget.h>
#include <memwidget.h>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
    void on_tabWidget_currentChanged(int);
    void process_update();
    void state_update();
    void slot_sortByColumn(int);
   // void on_stuTableWidget_customContextMenuRequested(QPoint pos);
    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::MainWindow *ui;
    QTimer *timer,*ntimer;
    void process_refresh();
    void tableWidgetSort();
    void system_info_show();
    void state_refresh();
    CPUWidget *cpuwidget;
    MemWidget *memwidget;
    //QMenu *popMenu;
    //QAction *action;
};

#endif // MAINWINDOW_H
