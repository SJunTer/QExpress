TEMPLATE = app

CONFIG += c++11

SOURCES += \
    main.cpp \
    ../activate/activate.cpp \
    ../activate/hdinfo.cpp \
    ../security/sha256.cpp

SUBDIRS += \
    ../activate/login.pro

HEADERS += \
    ../activate/activate.h \
    ../activate/hdinfo.h \
    ../security/sha256.h
