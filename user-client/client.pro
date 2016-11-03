TARGET = client

TEMPLATE = app
CONFIG += c++11

QT += widgets

HEADERS += \
    mainwindow.h \
    mapwidget.h \
    logindlg.h

SOURCES += \
    mainwindow.cpp \
    main.cpp \
    mapwidget.cpp \
    logindlg.cpp

FORMS += \
    logindlg.ui
