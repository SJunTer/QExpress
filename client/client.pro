TARGET = client

TEMPLATE = app
CONFIG += c++11

QT += widgets

HEADERS += \
    mainwindow.h \
    ../network/commands.h \
    ../network/packet.h \
    ../network/socket.h \
    ../security/des.h \
    logindlg.h \
    mapview.h \
    pixmapitem.h

SOURCES += \
    mainwindow.cpp \
    main.cpp \
    ../network/packet.cpp \
    ../network/socket.cpp \
    ../security/des.cpp \
    logindlg.cpp \
    mapview.cpp \
    pixmapitem.cpp

FORMS +=
