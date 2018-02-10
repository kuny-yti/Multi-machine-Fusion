INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
QT += network

HEADERS += \
    $$PWD/socket_thread_tcp.h \
    $$PWD/server_tcp.h \
    $$PWD/s_tcp_implement.h
SOURCES += \
    $$PWD/socket_thread_tcp.cpp \
    $$PWD/server_tcp.cpp \
    $$PWD/s_tcp_implement.cpp
