INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
QT += network

HEADERS += \
    $$PWD/client_tcp.h \
    $$PWD/c_tcp_implement.h
SOURCES += \
    $$PWD/client_tcp.cpp \
    $$PWD/c_tcp_implement.cpp
