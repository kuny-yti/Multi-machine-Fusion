INCLUDEPATH += $$PWD
VPATH += $$PWD

INCLUDEPATH += $$PWD/include
INCLUDEPATH += $$PWD/include/AL
INCLUDEPATH += $$PWD/OpenAL32/Include

DEFINES += AL_LIBTYPE_STATIC
DEFINES += AL_ALEXT_PROTOTYPES

HEADERS += \
    $$PWD/include/AL/al.h \
    $$PWD/include/AL/alc.h \
    $$PWD/include/AL/alext.h \
    $$PWD/include/AL/efx.h \
    $$PWD/include/AL/efx-creative.h \
    $$PWD/include/AL/efx-presets.h \
    $$PWD/Alc/mixer_defs.h \
    $$PWD/OpenAL32/Include/alAuxEffectSlot.h \
    $$PWD/OpenAL32/Include/alBuffer.h \
    $$PWD/OpenAL32/Include/alEffect.h \
    $$PWD/OpenAL32/Include/alError.h \
    $$PWD/OpenAL32/Include/alFilter.h \
    $$PWD/OpenAL32/Include/alListener.h \
    $$PWD/OpenAL32/Include/alMain.h \
    $$PWD/OpenAL32/Include/alSource.h \
    $$PWD/OpenAL32/Include/alThunk.h \
    $$PWD/OpenAL32/Include/alu.h \
    $$PWD/OpenAL32/Include/bs2b.h \
    $$PWD/config.h

SOURCES += \
    #$$PWD/Alc/backends/dsound.c \
    $$PWD/Alc/backends/loopback.c \
    #$$PWD/Alc/backends/mmdevapi.c \
    $$PWD/Alc/backends/null.c \
    $$PWD/Alc/backends/wave.c \
    $$PWD/Alc/backends/winmm.c \
    $$PWD/Alc/ALc.c \
    $$PWD/Alc/alcConfig.c \
    $$PWD/Alc/alcDedicated.c \
    $$PWD/Alc/alcEcho.c \
    $$PWD/Alc/alcModulator.c \
    $$PWD/Alc/alcReverb.c \
    $$PWD/Alc/alcRing.c \
    $$PWD/Alc/alcThread.c \
    $$PWD/Alc/ALu.c \
    $$PWD/Alc/bs2b.c \
    $$PWD/Alc/helpers.c \
    $$PWD/Alc/hrtf.c \
    $$PWD/Alc/mixer.c \
    $$PWD/Alc/mixer_c.c \
    $$PWD/Alc/mixer_inc.c \
    $$PWD/Alc/mixer_sse.c \
    $$PWD/Alc/panning.c \
    $$PWD/OpenAL32/alAuxEffectSlot.c \
    $$PWD/OpenAL32/alBuffer.c \
    $$PWD/OpenAL32/alEffect.c \
    $$PWD/OpenAL32/alError.c \
    $$PWD/OpenAL32/alExtension.c \
    $$PWD/OpenAL32/alFilter.c \
    $$PWD/OpenAL32/alListener.c \
    $$PWD/OpenAL32/alSource.c \
    $$PWD/OpenAL32/alState.c \
    $$PWD/OpenAL32/alThunk.c
    #$$PWD/utils/makehrtf.c


QMAKE_CXXFLAGS +=  -mmmx -msse
QMAKE_CFLAGS +=  -mmmx -msse

LIBS+=$$PWD/../ffmpeg/system/libwinmm.a
