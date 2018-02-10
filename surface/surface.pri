INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

QT       += opengl

HEADERS += \
SOURCES += \
FORMS += \
    $$PWD/ffd_property.h \
    $$PWD/ffd_surface.h \
    surface/bezier_arithmetic.h \
    surface/render_property.h \
    surface/ffd_render.h \
    surface/fusion_render.h \
    surface/ffd_group.h \
    surface/ffd_multi.h

SOURCES += \
    $$PWD/ffd_surface.cpp \
    surface/bezier_arithmetic.cpp \
    surface/ffd_render.cpp \
    surface/fusion_render.cpp \
    surface/ffd_group.cpp \
    surface/ffd_multi.cpp

