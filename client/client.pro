TARGET = client

TEMPLATE = app
CONFIG += c++11

QT += widgets


INCLUDEPATH += ../network
INCLUDEPATH += ../security
INCLUDEPATH += login

FORMS += \
    login/settingdlg.ui \
    login/regdlg.ui \
    login/logindlg.ui \
    aboutdlg.ui \
    taskwidget.ui \
    userwidget.ui

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
    taskwidget.h \
    userwidget.h

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
    taskwidget.cpp \
    userwidget.cpp

RESOURCES += \
    icon.qrc


