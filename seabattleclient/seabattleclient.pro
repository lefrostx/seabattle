#-------------------------------------------------
#
# Project created by QtCreator 2016-08-24T16:48:51
#
#-------------------------------------------------

QT       += core gui network xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = seabattleclient
TEMPLATE = app

CONFIG += c++14

SOURCES += main.cpp\
        mainwindow.cpp \
    box.cpp \
    map.cpp \
    ocean.cpp \
    server.cpp \

HEADERS  += mainwindow.h \
    box.h \
    map.h \
    mathematic.h \
    ocean.h \
    server.h

FORMS    += mainwindow.ui

RESOURCES += \
    resource.qrc
