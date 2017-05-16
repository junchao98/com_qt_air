#-------------------------------------------------
#
# Project created by QtCreator 2017-03-27T18:49:21
#
#-------------------------------------------------

QT       += core gui
QT       += serialport
QT       +=sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = air_com
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui
