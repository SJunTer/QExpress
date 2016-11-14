TARGET = QExpress

TEMPLATE = app
CONFIG += c++11

QT += widgets

LIBS += -L$$PWD/./mitab/ -lmitab
LIBS += -L$$PWD/./mitab/ -logr
LIBS += -L$$PWD/./mitab/ -lcpl

INCLUDEPATH += ./mitab

SOURCES += \
    main.cpp \
    optimumPath.cpp \
    tabwidget.cpp \
    mainwindow.cpp \
    mapviewer/mapfield.cpp \
    mapviewer/maplayer.cpp \
    mapviewer/mapshape.cpp \
    mapviewer/mapview.cpp \
    deliverywidget.cpp \
    mapviewer/mapwidget.cpp \
    deliverypath.cpp \
    truckwidget.cpp \
    inventorywidget.cpp \
    mapviewer/gstfile.cpp \
    mapviewer/miffile.cpp \
    server/serverthread.cpp \
    ../network/packet.cpp \
    ../network/socket.cpp \
    server/connection.cpp \
    ../security/des.cpp \
    mapviewer/tilesdlg.cpp \
    accwidget.cpp

HEADERS += \
    optimumPath.h \
    tabwidget.h \
    mainwindow.h \
    mapviewer/mapfield.h \
    mapviewer/maplayer.h \
    mapviewer/mapshape.h \
    mapviewer/mapview.h \
    deliverywidget.h \
    mapviewer/mapwidget.h \
    deliverypath.h \
    truckwidget.h \
    nofocusdelegate.h \
    inventorywidget.h \
    mapviewer/gstfile.h \
    mapviewer/miffile.h \
    server/serverthread.h \
    ../network/commands.h \
    ../network/packet.h \
    ../network/socket.h \
    server/connection.h \
    ../security/des.h \
    mapviewer/tilesdlg.h \
    accwidget.h

RESOURCES += \
    map.qrc

DISTFILES += \
    README.md

