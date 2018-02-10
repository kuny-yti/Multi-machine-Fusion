
INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

QT += core gui printsupport

HEADERS += \
    $$PWD/RegistryKey.h \
    $$PWD/Threads.h \
    $$PWD/ScreenDriverInterface.h \
    $$PWD/MirrorDriver.h \
    $$PWD/ScreenClient.h \
    $$PWD/Desktop.h

SOURCES += \
    $$PWD/Threads.cpp \
    $$PWD/RegistryKey.cpp \
    $$PWD/MirrorDriver.cpp \
    $$PWD/ScreenClient.cpp \
    $$PWD/Desktop.cpp

FORMS    += \

LIBS += -lgdi32
