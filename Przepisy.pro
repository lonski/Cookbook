#-------------------------------------------------
#
# Project created by QtCreator 2013-12-16T18:16:08
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Przepisy
TEMPLATE = app

RC_FILE = app.rc

SOURCES += main.cpp\
        przepisy.cpp \
        db/database.cpp \
        sqlite3.c

HEADERS  += przepisy.h \
            db/database.h \
            sqlite3.h

FORMS    += przepisy.ui

RESOURCES += \
    icons.qrc

LIBS += -lpthread -ldl
