#-------------------------------------------------
#
# Project created by QtCreator 2016-09-06T09:43:11
#
#-------------------------------------------------

QT       += network

QT       -= gui

OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
RCC_DIR=build

DESTDIR = bin

TARGET = SaleaeSocketApi
TEMPLATE = lib

include(SaleaeSocketApi.pri)

unix {
    target.path = /usr/lib
    INSTALLS += target
}
