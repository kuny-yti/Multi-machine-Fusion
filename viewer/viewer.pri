
INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

QT += core gui network opengl

HEADERS += \
    viewer/camera.h \
    viewer/window_base.h \
    viewer/context_thread.h \
    viewer/window_view.h

SOURCES += \
    viewer/camera.cpp \
    viewer/window_base.cpp \
    viewer/context_thread.cpp \
    viewer/window_view.cpp

