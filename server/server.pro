TARGET = server

TEMPLATE = app

CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    packet.cpp \
    server.cpp \
    socket.cpp

HEADERS += \
    packet.h \
    socket.h \
    commands.h
