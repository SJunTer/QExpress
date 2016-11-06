TARGET = client

TEMPLATE = app

CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    client.cpp \
    packet.cpp \
    socket.cpp

HEADERS += \
    packet.h \
    socket.h
