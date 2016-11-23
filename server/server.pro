TARGET = QExpress

TEMPLATE = app
CONFIG += c++11

QT += widgets


LIBS += -L$$PWD/mitab/ -lmitab
LIBS += -L$$PWD/database/mysql -lmysqlclient

INCLUDEPATH += mitab


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
    mapviewer/gstfile.cpp \
    mapviewer/miffile.cpp \
    ../network/packet.cpp \
    ../network/socket.cpp \
    ../security/des.cpp \
    ../security/md5.cpp \
    mapviewer/tilesdlg.cpp \
    server/connection.cpp \
    server/tcpserver.cpp \
    accwidget.cpp \
    mapviewer/pixmapitem.cpp \
    aboutdlg.cpp \
    actdlg.cpp \
    ../activate/activate.cpp \
    ../activate/hdinfo.cpp \
    database/operatesql.cpp \
    ../security/sha256.cpp \
    messagedlg.cpp \
    cargowidget.cpp

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
    mapviewer/gstfile.h \
    mapviewer/miffile.h \
    ../network/commands.h \
    ../network/packet.h \
    ../network/socket.h \
    ../security/des.h \
    ../security/md5.h \
    mapviewer/tilesdlg.h \
    server/connection.h \
    server/tcpserver.h \
    accwidget.h \
    mapviewer/pixmapitem.h \
    aboutdlg.h \
    actdlg.h \
    ../activate/activate.h \
    ../activate/hdinfo.h \
    database/operatedefine.h \
    database/operatesql.h \
    ../security/sha256.h \
    messagedlg.h \
    cargowidget.h

RESOURCES += \
    map.qrc

DISTFILES += \
    README.md

FORMS += \
    aboutdlg.ui \
    actdlg.ui \
    messagedlg.ui


