#-------------------------------------------------
#
# Project created by QtCreator 2014-04-06T18:47:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = readmotiv
TEMPLATE = app


SOURCES += src/main.cpp\
        src/mainwindow.cpp \
    src/library.cpp \
    src/statistics.cpp

HEADERS  += src/mainwindow.h \
    src/library.h \
    src/statistics.h

FORMS    += src/mainwindow.ui
