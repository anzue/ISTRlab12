#-------------------------------------------------
#
# Project created by QtCreator 2017-04-15T11:30:46
#
#-------------------------------------------------

QT       += core gui sql

CONFIG += c++14

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MySQLControlSystem
TEMPLATE = app


SOURCES += main.cpp\
        mainform.cpp \
    databaseoperator.cpp \
    qcustomplot.cpp

HEADERS  += mainform.h \
    databaseoperator.h \
    qcustomplot.h

FORMS    += mainform.ui

#INCLUDEPATH += /opt/Qt/5.3/Src/qtbase/include/
