INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
QT += network

HEADERS += \
    $$PWD/tcp_interfaces.h \
    $$PWD/tcp_comm_implement.h
SOURCES += \
    $$PWD/tcp_comm_implement.cpp
