
INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

QT       += opengl

HEADERS += \
    render_api/ray_pick.h \
    render_api/render_kernel.h \
    render_api/type_define.h \
    #render_api/base_render.h \
    render_api/polyline.h \
    render_api/texture_upload.h

SOURCES += \
    render_api/ray_pick.cpp \
    render_api/render_kernel.cpp \
    #render_api/base_render.cpp \
    render_api/polyline.cpp \
    render_api/type_define.cpp \
    render_api/texture_upload.cpp



LIBS += -lopengl32 -lglu32
