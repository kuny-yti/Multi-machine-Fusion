
QT       += network

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

SOURCES += \
    $$PWD/AVClock.cpp \
    $$PWD/network_sync.cpp \
    $$PWD/time_server.cpp \
    $$PWD/serialize.cpp

HEADERS  += \
    $$PWD/AVClock.h \
    $$PWD/network_sync.h \
    $$PWD/time_server.h \
    $$PWD/serialize.h
