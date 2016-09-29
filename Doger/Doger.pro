QT       += core gui charts svg sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Doger
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    listwindow.cpp \
    statisticswindow.cpp \
    sqlite.cpp \
    itemwindow.cpp

HEADERS  += mainwindow.h \
    listwindow.h \
    statisticswindow.h \
    sqlite.h \
    itemwindow.h

FORMS    += mainwindow.ui \
    listwindow.ui \
    statisticswindow.ui \
    itemwindow.ui

RESOURCES += \
    ressources.qrc
