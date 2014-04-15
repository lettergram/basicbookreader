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
        src/basicbookreader.cpp \
    src/library.cpp \
    src/statistics.cpp \

HEADERS  += src/basicbookreader.h \
    src/library.h \
    src/statistics.h \

FORMS    += src/basicbookreader.ui
