#-------------------------------------------------
#
# Project created by QtCreator 2013-05-02T23:03:02
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TaskManagerForLinux
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    cpuwidget.cpp \
    memwidget.cpp

HEADERS  += mainwindow.h \
    cpuwidget.h \
    memwidget.h

FORMS    += mainwindow.ui
