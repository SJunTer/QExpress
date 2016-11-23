TEMPLATE = app

CONFIG += c++11

SOURCES += \
    main.cpp \
    ../activate/activate.cpp \
    ../activate/hdinfo.cpp \
    ../activate/md5.cpp

SUBDIRS += \
    ../activate/login.pro

HEADERS += \
    ../activate/activate.h \
    ../activate/hdinfo.h \
    ../activate/md5.h
