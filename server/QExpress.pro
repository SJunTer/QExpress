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
    mapviewer/maplayer.cpp \
    mapviewer/mapshape.cpp \
    mapviewer/mapview.cpp \
    deliverywidget.cpp \
    mapviewer/mapwidget.cpp \
    deliverypath.cpp \
    truckwidget.cpp \
    inventorywidget.cpp \
    mapviewer/gstfile.cpp \
    mapviewer/miffile.cpp

HEADERS += \
    optimumPath.h \
    des.h \
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
    mapviewer/miffile.h

RESOURCES += \
    map.qrc

DISTFILES += \
    README.md

