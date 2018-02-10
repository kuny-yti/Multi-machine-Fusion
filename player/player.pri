#-------------------------------------------------
#
# Project created by QtCreator 2014-07-14T22:31:33
#
#-------------------------------------------------

QT       += opengl network

INCLUDEPATH += $$PWD
VPATH += $$PWD

INCLUDEPATH += $$PWD/base/
INCLUDEPATH += $$PWD/lib/ffmpeg/
INCLUDEPATH += $$PWD/lib/ffmpeg/ffmpeg

LIBS+=$$PWD/lib/pthread/libpthread.a

#####################################################
LIBS+= $$PWD/lib/ffmpeg/ffmpeg/libavdevice.a
LIBS+= $$PWD/lib/ffmpeg/ffmpeg/libavfilter.a
LIBS+= $$PWD/lib/ffmpeg/ffmpeg/libavformat.a
LIBS+= $$PWD/lib/ffmpeg/ffmpeg/libavcodec.a
LIBS+= $$PWD/lib/ffmpeg/ffmpeg/libpostproc.a
LIBS+= $$PWD/lib/ffmpeg/ffmpeg/libswresample.a
LIBS+= $$PWD/lib/ffmpeg/ffmpeg/libswscale.a
LIBS+= $$PWD/lib/ffmpeg/ffmpeg/libavutil.a
LIBS+= $$PWD/lib/ffmpeg/ffmpeg/libx264.a
DEFINES += __STDC_CONSTANT_MACROS
#############################################

include (./lib/openal/openal.pri)

INCLUDEPATH += $$PWD/lib/libass/include
LIBS += $$PWD/lib/libass/libass.a


LIBS += $$PWD/lib/fribidi/libfribidi.a
LIBS += $$PWD/lib/enca/libenca.a
LIBS += $$PWD/lib/fontconfig/libfontconfig.a
LIBS += $$PWD/lib/harfbuzz/libharfbuzz.a
LIBS += $$PWD/lib/harfbuzz/libharfbuzz-icu.a

LIBS += $$PWD/lib/glib/libglib-2.0.a
LIBS += $$PWD/lib/expat/libexpat.a

INCLUDEPATH += $$PWD/lib/freetype/include
LIBS += $$PWD/lib/freetype/libfreetype.a
LIBS += $$PWD/lib/libintl/libintl.a


SOURCES += \
    $$PWD/audio_output.cpp \
    $$PWD/color_matrix.cpp \
    $$PWD/dispatch.cpp \
    $$PWD/media_data.cpp \
    $$PWD/media_input.cpp \
    $$PWD/media_object.cpp \
    $$PWD/player.cpp \
    $$PWD/subtitle_renderer.cpp \
    $$PWD/video_output.cpp \
    $$PWD/base/dbg.cpp \
    $$PWD/base/exc.cpp \
    $$PWD/base/msg.cpp \
    $$PWD/base/s11n.cpp \
    $$PWD/base/str.cpp \
    $$PWD/base/thread.cpp \
    $$PWD/base/timer_.cpp \
    player/input_handel.cpp \
    player/color_handel.cpp \
    player/subtitle_updater.cpp \
    player/subtitle_handel.cpp \
    player/render_handel.cpp

HEADERS  += \
    $$PWD/audio_output.h \
    $$PWD/color_matrix.h \
    $$PWD/dispatch.h \
    $$PWD/media_data.h \
    $$PWD/media_input.h \
    $$PWD/media_object.h \
    $$PWD/player.h \
    $$PWD/subtitle_renderer.h \
    $$PWD/video_output.h \
    $$PWD/base/blob.h \
    $$PWD/base/dbg.h \
    $$PWD/base/exc.h \
    $$PWD/base/gettext.h \
    $$PWD/base/intcheck.h \
    $$PWD/base/msg.h \
    $$PWD/base/s11n.h \
    $$PWD/base/str.h \
    $$PWD/base/thread.h \
    $$PWD/base/timer_.h \
    player/input_handel.h \
    player/color_handel.h \
    player/subtitle_updater.h \
    player/subtitle_handel.h \
    player/render_handel.h

OTHER_FILES += \
    $$PWD/video_output_color.fs.glsl \
    $$PWD/video_output_render.fs.glsl

LIBS+=$$PWD/lib/ffmpeg/system/libwsock32.a
LIBS+=$$PWD/lib/ffmpeg/system/libwinmm.a
LIBS+=$$PWD/lib/ffmpeg/system/libiconv.a
LIBS+=$$PWD/lib/ffmpeg/system/libvfw32.a
LIBS+=$$PWD/lib/ffmpeg/system/libgdi32.a
LIBS+=$$PWD/lib/ffmpeg/system/libole32.a
LIBS+=$$PWD/lib/ffmpeg/system/libws2_32.a


RESOURCES += \
    $$PWD/shader.qrc

