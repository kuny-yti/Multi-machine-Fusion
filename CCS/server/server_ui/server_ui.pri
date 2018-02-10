INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
!win32:QT += network
win32:LIBS += -luser32

HEADERS += \
    $$PWD/server_ui.h \
    $$PWD/list_addres.h \
    server_ui/pjlink.h \
    server_ui/list_node.h \
    server_ui/server_time.h
SOURCES += \
    $$PWD/server_ui.cpp \
    $$PWD/list_addres.cpp \
    server_ui/pjlink.cpp \
    server_ui/list_node.cpp \
    server_ui/server_time.cpp

FORMS += \
     $$PWD/server_ui.ui \
    server_ui/playernode.ui \
    server_ui/pjlink.ui
