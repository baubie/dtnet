#-------------------------------------------------
#
# Project created by QtCreator 2009-04-18T17:15:43
#
#-------------------------------------------------

QT       += xml

TARGET = qdtnet
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

QMAKE_CXXFLAGS = -pthread -I/usr/local/include/boost-1_37
QMAKE_LIBS = -lboost_system-xgcc40-mt -L/local/lib -ldtnet
