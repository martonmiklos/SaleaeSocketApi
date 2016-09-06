#-------------------------------------------------
#
# Project created by QtCreator 2016-09-06T15:45:01
#
#-------------------------------------------------

QT       += core gui network

OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
RCC_DIR=build

DESTDIR = bin

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QSaleaeSocketApiExample
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

INCLUDEPATH += ../QSaleaeSocketApi

LIBS += -L../QSaleaeSocketApi/bin -lSaleaeSocketApi
