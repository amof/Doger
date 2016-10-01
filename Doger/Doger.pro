QT       += core gui charts svg sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Doger
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    statisticswindow.cpp \
    sqlite.cpp \
    itemwindow.cpp \
    listwindow.cpp

HEADERS  += mainwindow.h \
    statisticswindow.h \
    sqlite.h \
    itemwindow.h \
    listwindow.h

FORMS    += mainwindow.ui \
    statisticswindow.ui \
    itemwindow.ui \
    listwindow.ui

RESOURCES += \
    ressources.qrc
