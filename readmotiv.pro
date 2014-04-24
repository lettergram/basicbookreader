#-------------------------------------------------
#
# Project created by QtCreator 2014-04-06T18:47:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = readmotiv
TEMPLATE = app
#CONFIG-=app_bundle

SOURCES += src/main.cpp\
    src/basicbookreader.cpp \
    src/library.cpp \
    src/statistics.cpp \
    src/statsviewer.cpp \

HEADERS  += src/basicbookreader.h \
    src/library.h \
    src/statistics.h \
    src/statsviewer.h \

FORMS    += src/basicbookreader.ui
FORMS    += src/statsviewer.ui
