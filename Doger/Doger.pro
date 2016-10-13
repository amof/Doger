QT       += core gui charts svg sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Doger
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    sqlite.cpp \
    itemwindow.cpp \
    listwindow.cpp \
    statisticswindow.cpp

HEADERS  += mainwindow.h \
    sqlite.h \
    itemwindow.h \
    listwindow.h \
    statisticswindow.h

FORMS    += mainwindow.ui \
    itemwindow.ui \
    listwindow.ui \
    statisticswindow.ui

RESOURCES += \
    ressources.qrc
