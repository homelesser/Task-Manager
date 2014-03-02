#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QMessageBox>
#include <QDir>
#include <QListWidget>
#include <QListWidgetItem>
#include <QStringList>
#include <QTimer>
#include <QVector>
#include <sstream>
#include <QPainter>
//#include <unistd.h>

QFile tempfile;
QString tempstr;
QVector<QVector<QString> > v;
extern QVector<double> vcpu, vmem;
int col = 0, rev = 0;

MainWindow::MainWindow(QWidget *parent) :
QMainWindow(parent),
ui(new Ui::MainWindow) {
    ui->setupUi(this);
    timer = new QTimer(this);
    ntimer = new QTimer(this);
    ntimer -> start(1000);
    timer->start(5000);
    QWidget::connect(ntimer, SIGNAL(timeout()), this, SLOT(state_update()));
    QWidget::connect(timer, SIGNAL(timeout()), this, SLOT(process_update()));
    QHeaderView* headerView = ui->tableWidget->verticalHeader();
    headerView->setHidden(true);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tabWidget->setTabText(0, QString("Process"));
    ui->tabWidget->setTabText(1, QString("State"));
    ui->tabWidget->setTabText(2, QString("System"));
    ui->tabWidget->setTabText(3, QString("About"));
    //ui->tableWidget->setSortingEnabled(false);
    ui->tableWidget->setColumnCount(5);
    QWidget::connect(ui->tableWidget->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(slot_sortByColumn(int)));
    QRegExp regExp2("[1-9]{1,1}[0-9]{0,3}");
    QRegExpValidator *pRegExpValidator2 = new QRegExpValidator(regExp2, this);
    ui->lineEdit->setValidator(pRegExpValidator2);
    cpuwidget = new CPUWidget;
    cpuwidget->setParent(ui->tabWidget);
    cpuwidget->setGeometry(40, 80, 450, 100);
    memwidget = new MemWidget;
    memwidget->setParent(ui->tabWidget);
    memwidget->setGeometry(40 ,220, 450, 100);

    v.clear();
    vcpu.clear();
    process_refresh();
    this->cpuwidget->hide();
    this->memwidget->hide();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::tableWidgetSort() {
    if (rev == 0) ui->tableWidget->sortItems(col, Qt::AscendingOrder);
    else ui->tableWidget->sortItems(col, Qt::DescendingOrder);
}

void MainWindow::slot_sortByColumn(int index) {
    if (index == col) rev ^= 1;
    else {
        col = index;
        rev = 0;
    }
    tableWidgetSort();
}

void MainWindow::process_update() {
    if (ui->tabWidget->currentIndex() == 0) process_refresh();
}

void MainWindow::state_update() {
    if (ui->tabWidget->currentIndex() == 1) state_refresh();
}

void MainWindow::process_refresh() {
    this->cpuwidget->hide();
    this->memwidget->hide();
    QDir qd("/proc");
    QStringList qsList = qd.entryList();
    QString qs = qsList.join("\n");
    QString id_of_pro;
    bool ok;
    int find_start = 3;
    int a, b;
    int number_of_sleep = 0, number_of_run = 0, number_of_zombie = 0;
    int totalProNum = 0;
    QString proName, proState, proPri, proMemRES, proMemSHR;
    v.clear();
    while (1) {
        a = qs.indexOf("\n", find_start);
        b = qs.indexOf("\n", a + 1);
        find_start = b;
        id_of_pro = qs.mid(a + 1, b - a - 1);
        totalProNum++;
        id_of_pro.toInt(&ok, 10);
        if (!ok) break;
        tempfile.setFileName("/proc/" + id_of_pro + "/stat");
        if (!tempfile.open(QIODevice::ReadOnly)) {
            //QMessageBox::warning(this, tr("warning"), tr("The pid stat file can not open!"), QMessageBox::Yes);
            return;
        }
        tempstr = tempfile.readLine();
        tempfile.close();
        if (tempstr.length() == 0) break;
        a = tempstr.indexOf("(");
        b = tempstr.indexOf(")");
        proName = tempstr.mid(a + 1, b - a - 1);
        proName.trimmed();
        proState = tempstr.section(" ", 2, 2);
        proPri = tempstr.section(" ", 17, 17);
        proMemRES = tempstr.section(" ", 23, 23);
        tempfile.setFileName("/proc/" + id_of_pro + "/statm");
        if (!tempfile.open(QIODevice::ReadOnly)) {
            //QMessageBox::warning(this, tr("warning"), tr("The pid stat file can not open!"), QMessageBox::Yes);
            return;
        }
        tempstr = tempfile.readLine();
        tempfile.close();

        proMemSHR = tempstr.section(" ", 2, 2);
        switch (proState.at(0).toLatin1()) {
            case 'S':
                number_of_sleep++;
                break;
            case 'R':
                number_of_run++;
                break;
            case 'Z':
                number_of_zombie++;
                break;
        }
        QVector<QString> vv;
        vv.push_back(id_of_pro);
        vv.push_back(proName);
        vv.push_back(proState);
        vv.push_back(proPri);
        vv.push_back(proMemRES);
        vv.push_back(proMemSHR);
        v.push_back(vv);
    }
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(v.size());
    int cntR = 0, cntS = 0, cntD = 0, cntT = 0, cntZ = 0;
    for (int i = 0; i < v.size(); i++) {
        for (int j = 0; j < ui->tableWidget->columnCount(); j++) {
            QTableWidgetItem *item = new QTableWidgetItem();
            if (j == 0 || j == 3 || j == 4) {
                if (j == 4) {
                    double res = 1e99, shr = 1e99;
                    res = v[i][j].toDouble(&ok);
                    shr = v[i][j + 1].toDouble(&ok);
                    item->setData(Qt::DisplayRole, (res - shr) / 256.0);
                } else {
                    int val = 2147483647;
                    val = v[i][j].toInt(&ok, 10);
                    item->setData(Qt::DisplayRole, val);
                }
            } else item->setText(v[i][j]);
            ui->tableWidget->setItem(i, j, item);
        }
        if (v[i][2] == "D") cntD++;
        if (v[i][2] == "R") cntR++;
        if (v[i][2] == "S") cntS++;
        if (v[i][2] == "T") cntT++;
        if (v[i][2] == "Z") cntZ++;
    }
    tableWidgetSort();
    ui->label_2->setText(QString("Tot:\t") + QString("").sprintf("%d\n", v.size())
            + QString("D:\t") + QString("").sprintf("%d\n", cntD)
            + QString("R:\t") + QString("").sprintf("%d\n", cntR)
            + QString("S:\t") + QString("").sprintf("%d\n", cntS)
            + QString("T:\t") + QString("").sprintf("%d\n", cntT)
            + QString("Z:\t") + QString("").sprintf("%d", cntZ)
            );
}

void MainWindow::state_refresh() {
    //    ui->label_3->setText();
    //    ui->label_4->setText();
    this->cpuwidget->show();
    this->memwidget->show();
    double tot[2] = {0, 0}, idle[2] = {0, 0};
    for (int tt = 0; tt < 2; tt++) {
        tempfile.setFileName("/proc/stat");
        if (!tempfile.open(QIODevice::ReadOnly)) {
            QMessageBox::warning(this, tr("warning"), tr("The stat file can not open!"), QMessageBox::Yes);
            return;
        }
        tempstr = tempfile.readLine();
        tempfile.close();
        std::stringstream sin;
        sin << tempstr.toStdString();
        std::string tmp;
        sin >> tmp;
        int x;
        for (int i = 0; i < 9; i++) {
            sin >> x;
            if (i == 3) idle[tt] = x;
            tot[tt] += x;
        }
        usleep(100 * 1000);
    }
    double pcpu = 1 - (idle[1] - idle[0]) / (tot[1] - tot[0]);
    ui->label_3->setText(QString("CPU: ") + QString("").sprintf("%.2f%%", pcpu * 100));
    vcpu.push_back(pcpu);

    tempfile.setFileName("/proc/meminfo"); //打开内存信息文件
    if (!tempfile.open(QIODevice::ReadOnly)) {
        //     QMessageBox::warning(this, tr("warning"), tr("The meminfo file can not open!"), QMessageBox::Yes);
        return;
    }
    QString memTotal;
    QString memFree;
    QString memUsed;
    QString swapTotal;
    QString swapFree;
    QString swapUsed;
    double nMemTotal, nMemFree, nMemUsed, nSwapTotal, nSwapFree, nSwapUsed;
    int pos = -1;
    while (1) {
        tempstr = tempfile.readLine();
        pos = tempstr.indexOf("MemTotal");
        if (pos != -1) {
            memTotal = tempstr.mid(pos + 10, tempstr.length() - 13);
            memTotal = memTotal.trimmed();
            nMemTotal = memTotal.toInt() / 1024.0;
        } else if (pos = tempstr.indexOf("MemFree"), pos != -1) {
            memFree = tempstr.mid(pos + 9, tempstr.length() - 12);
            memFree = memFree.trimmed();
            nMemFree = memFree.toInt() / 1024.0;
        } else if (pos = tempstr.indexOf("SwapTotal"), pos != -1) {
            swapTotal = tempstr.mid(pos + 11, tempstr.length() - 14);
            swapTotal = swapTotal.trimmed();
            nSwapTotal = swapTotal.toInt() / 1024.0;
        } else if (pos = tempstr.indexOf("SwapFree"), pos != -1) {
            swapFree = tempstr.mid(pos + 10, tempstr.length() - 13);
            swapFree = swapFree.trimmed();
            nSwapFree = swapFree.toInt() / 1024.0;
            break;
        }
    }
    nMemUsed = nMemTotal - nMemFree;
    double pmem = nMemUsed / nMemTotal;
    ui->label_4->setText(QString("Memory: ")
            + QString("").sprintf("%.2f%% %.2fMB", pmem * 100, nMemUsed)
            );
    vmem.push_back(pmem);

    tempfile.close();
    this->cpuwidget->update();
    this->memwidget->update();
}

void MainWindow::system_info_show() {
    char s[100];
    gethostname(s, sizeof (s));
    QString text;
    text += QString("Hostname :\t\t");
    text += QString(s);
    text += QString("\n\n");
    tempfile.setFileName("/proc/version");
    if (!tempfile.open(QIODevice::ReadOnly)) {
        //QMessageBox::warning(this, tr("warning"), tr("The pid stat file can not open!"), QMessageBox::Yes);
        return;
    }
    tempstr = tempfile.readLine();
    tempfile.close();
    std::stringstream sin;
    sin<<tempstr.toStdString();
    sin>>s;
    text+=s;
    text+=" ";
    sin>>s;
    text+=s;
    text+=" :\t";
    sin>>s;
    text+=s;
    text+="\n\n";
    tempfile.setFileName("/proc/cpuinfo");
    if (!tempfile.open(QIODevice::ReadOnly)) {
        //QMessageBox::warning(this, tr("warning"), tr("The pid stat file can not open!"), QMessageBox::Yes);
        return;
    }
    int cnt = 0;
    while (1){
        tempstr = tempfile.readLine();
        if (tempstr=="") break;
        cnt++;
        if (cnt==5){
            text+="CPU info :\t\t";
            int pos = tempstr.indexOf(":");
            text+=tempstr.mid(pos+2);
        }
    }
    tempfile.close();
    text+="\nTotal processor :\t";
    text+=QString("").sprintf("%d\n\n",(cnt+1)/26);
    system("df -Thl > a.txt\n");
    tempfile.setFileName("a.txt");
    if (!tempfile.open(QIODevice::ReadOnly)) {
        //QMessageBox::warning(this, tr("warning"), tr("The pid stat file can not open!"), QMessageBox::Yes);
        return;
    }
    while (1) {
        tempstr = tempfile.readLine();
        if (tempstr=="") break;
        std::stringstream ssin;
        ssin<<tempstr.toStdString();
        int cnt = 0;
        while (ssin>>s){
            if (cnt!=3&&cnt!=4){
                text+=s;
                text+="\t";
            }
            cnt++;
        }
        text+="\n";
    }
    tempfile.close();
    ui->textBrowser_2->setText(text);
}

void MainWindow::on_tabWidget_currentChanged(int index) {
    if (index == 0) {
        col = rev = 0;
        ui->label->show();
        ui->lineEdit->show();
        ui->pushButton->show();
        ui->pushButton_3->show();
        ui->label_2->show();
        process_refresh();
    } else {
        ui->label->hide();
        ui->lineEdit->hide();
        ui->pushButton->hide();
        ui->pushButton_3->hide();
        ui->label_2->hide();
        if (index == 1) {
            vcpu.clear();
            vmem.clear();
            state_refresh();
        } else {
            cpuwidget->hide();
            memwidget->hide();
            if (index == 2) {
                system_info_show();
            }
        }
    }
}

void MainWindow::on_pushButton_clicked() {
    bool ok;
    int row = ui->tableWidget->currentRow();
    int pid = ui->tableWidget->item(row, 0)->text().toInt(&ok);
    char s[100], w[100], r[100], e[100];
    sprintf(s, "kill %d\n", pid);
    sprintf(w, "Sure to kill %s ?", ui->tableWidget->item(row, 1)->text().toStdString().c_str());
    sprintf(r, "%s has been killed.", ui->tableWidget->item(row, 1)->text().toStdString().c_str());
    sprintf(e, "%s can\'t be killed.", ui->tableWidget->item(row, 1)->text().toStdString().c_str());
    if (QMessageBox::warning(NULL, "Warning", w, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes) {
        int ret = system(s);
        if (ret != 256) QMessageBox::information(NULL, "Complete", r, QMessageBox::Yes, QMessageBox::Yes);
        else QMessageBox::critical(NULL, "Error", e, QMessageBox::Yes, QMessageBox::Yes);
    }
}

void MainWindow::on_pushButton_3_clicked() {
    bool ok;
    this->timer->start(ui->lineEdit->text().toInt(&ok));
}
