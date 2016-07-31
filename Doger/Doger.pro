#-------------------------------------------------
#
# Project created by QtCreator 2016-07-25T10:30:18
#
#-------------------------------------------------

QT       += core gui charts svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Doger
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    listwindow.cpp \
    statisticswindow.cpp \
    itemwindow.cpp

HEADERS  += mainwindow.h \
    listwindow.h \
    statisticswindow.h \
    itemwindow.h

FORMS    += mainwindow.ui \
    listwindow.ui \
    statisticswindow.ui \
    itemwindow.ui

RESOURCES += \
    ressources.qrc
