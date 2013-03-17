#-------------------------------------------------
#
# Project created by QtCreator 2013-03-13T13:39:04
#
#-------------------------------------------------

TARGET = test_client
TEMPLATE = app

QT += core gui dbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets



CONFIG += link_pkgconfig

PKGCONFIG += global_action_native_client

SOURCES += \
    main.cpp\
    mainwindow.cpp \
    shortcut_selector.cpp

HEADERS += \
    mainwindow.hpp \
    shortcut_selector.hpp

FORMS += \
    mainwindow.ui
