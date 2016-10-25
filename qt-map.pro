TARGET = qt-map

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
    des.cpp \
    tabwidget.cpp \
    mainwindow.cpp \
    mapviewer/mapfield.cpp \
    mapviewer/mapfile.cpp \
    mapviewer/maplayer.cpp \
    mapviewer/mapshape.cpp \
    mapviewer/mapview.cpp \
    deliverywidget.cpp \
    mapviewer/mapwidget.cpp \
    deliverypath.cpp \
    truckwidget.cpp \
    inventorywidget.cpp

HEADERS += \
    optimumPath.h \
    des.h \
    tabwidget.h \
    mainwindow.h \
    mapviewer/mapfield.h \
    mapviewer/mapfile.h \
    mapviewer/maplayer.h \
    mapviewer/mapshape.h \
    mapviewer/mapview.h \
    deliverywidget.h \
    mapviewer/mapwidget.h \
    deliverypath.h \
    truckwidget.h \
    nofocusdelegate.h \
    inventorywidget.h

RESOURCES += \
    map.qrc

DISTFILES += \
    README.md

