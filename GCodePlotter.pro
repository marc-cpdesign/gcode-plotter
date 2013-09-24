#-------------------------------------------------
#
#-------------------------------------------------

QT += core gui
QT += opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GCodePlotter
TEMPLATE = app

HEADERS = \
    renderwidget.h \
    gcpmainwindow.h

SOURCES = \
    main.cpp \
    renderwidget.cpp \
    gcpmainwindow.cpp

unix {
    LIBS += -lGLU
}
