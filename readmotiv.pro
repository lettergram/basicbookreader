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
        src/bookreader/basicbookreader.cpp \
    src/bookreader/library.cpp \
    src/bookreader/statistics.cpp \
    src/statsviewer/statsviewer.cpp \

HEADERS  += src/bookreader/basicbookreader.h \
    src/bookreader/library.h \
    src/bookreader/statistics.h \
    src/statsviewer/statsviewer.h \

FORMS    += src/bookreader/basicbookreader.ui
FORMS    += src/statsviewer/statsviewer.ui
