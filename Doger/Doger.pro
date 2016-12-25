QT       += core gui charts svg sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Doger
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    sqlite.cpp \
    itemwindow.cpp \
    statisticswindow.cpp \
    listwindow.cpp

HEADERS  += mainwindow.h \
    sqlite.h \
    itemwindow.h \
    statisticswindow.h \
    listwindow.h

FORMS    += mainwindow.ui \
    itemwindow.ui \
    statisticswindow.ui \
    listwindow.ui

RESOURCES += \
    ressources.qrc
