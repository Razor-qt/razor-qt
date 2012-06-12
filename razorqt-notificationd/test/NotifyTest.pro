#-------------------------------------------------
#
# Project created by QtCreator 2012-06-05T22:26:57
#
#-------------------------------------------------

QT       += core gui dbus

TARGET = NotifyTest
TEMPLATE = app
INCLUDEPATH += ../../libraries/

SOURCES += main.cpp \
        mainwindow.cpp

HEADERS += mainwindow.h

FORMS += mainwindow.ui

LIBS += -lrazorqt
