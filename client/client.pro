TARGET = client

TEMPLATE = app
CONFIG += c++11

QT += widgets


INCLUDEPATH += ../network
INCLUDEPATH += ../security


HEADERS += \
    mainwindow.h \
    ../network/commands.h \
    ../network/packet.h \
    ../network/socket.h \
    ../security/des.h \
    logindlg.h \
    mapview.h \
    pixmapitem.h \
    pathwidget.h \
    regdlg.h \
    userdlg.h

SOURCES += \
    mainwindow.cpp \
    main.cpp \
    ../network/packet.cpp \
    ../network/socket.cpp \
    ../security/des.cpp \
    logindlg.cpp \
    mapview.cpp \
    pixmapitem.cpp \
    pathwidget.cpp \
    regdlg.cpp \
    userdlg.cpp

FORMS +=
