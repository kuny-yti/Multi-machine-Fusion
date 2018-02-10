
INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

QT       += opengl

HEADERS += \
    glsl_image/fbo_image.h

SOURCES += \
    glsl_image/fbo_image.cpp

RESOURCES += \
    glsl_image/glsl.qrc

OTHER_FILES += \
    glsl_image/vertex.gl.glsl \
    glsl_image/image.gl.glsl



