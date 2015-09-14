#-------------------------------------------------
#
# Project created by QtCreator 2015-09-13T14:56:43
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = brANPR
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    anprengine.cpp

HEADERS  += mainwindow.h \
    anprengine.h

FORMS    += mainwindow.ui

INCLUDEPATH += C:\dev\opencv\build\include

LIBS += -LC:\dev\opencv\build\x64\vc12\lib \
-lopencv_core2411d \
-lopencv_highgui2411d \
-lopencv_imgproc2411d \
-lopencv_video2411d \

