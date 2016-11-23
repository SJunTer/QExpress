TARGET = QExpress

TEMPLATE = app
CONFIG += c++11

QT += widgets


FORMS += \
    login/settingdlg.ui \
    login/regdlg.ui \
    login/logindlg.ui \
    aboutdlg.ui \
    taskdlg.ui \
    uploaddlg.ui \
    userdlg.ui \
    pwddlg.ui

HEADERS += \
    mainwindow.h \
    mapview.h \
    pathwidget.h \
    pixmapitem.h \
    account.h \
    login/logindlg.h \
    login/regdlg.h \
    login/settingdlg.h \
    ../network/commands.h \
    ../network/packet.h \
    ../network/socket.h \
    ../security/des.h \
    aboutdlg.h \
    tileloader.h \
    taskdlg.h \
    uploaddlg.h \
    userdlg.h \
    deliverypath.h \
    pwddlg.h

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    mapview.cpp \
    pathwidget.cpp \
    pixmapitem.cpp \
    login/logindlg.cpp \
    login/regdlg.cpp \
    login/settingdlg.cpp \
    ../network/packet.cpp \
    ../network/socket.cpp \
    ../security/des.cpp \
    aboutdlg.cpp \
    tileloader.cpp \
    taskdlg.cpp \
    uploaddlg.cpp \
    userdlg.cpp \
    pwddlg.cpp

RESOURCES += \
    icon.qrc


