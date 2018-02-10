
INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

win32 {
    DEFINES += _WINDOWS_SYSTEM_
    LIBSPATH = $$PWD/../bulid

    SOURCES += \
        $$PWD/private/windows/keyboard_p.cpp \
        $$PWD/private/windows/mouse_p.cpp \
        $$PWD/private/windows/command_p.cpp \
        $$PWD/private/windows/xp_volume_p.cpp \
        $$PWD/private/windows/type_p.cpp \
        $$PWD/private/windows/win7_volume_p.cpp

    HEADERS  += \
        $$PWD/private/windows/keyboard_p.h \
        $$PWD/private/windows/mouse_p.h \
        $$PWD/private/windows/command_p.h \
        $$PWD/private/windows/xp_volume_p.h \
        $$PWD/private/windows/type_p.h \
        $$PWD/private/windows/win7_volume_p.h \
        $$PWD/private/windows/KEY_W.h

    LIBS += $${LIBSPATH}/libwinmm.a
}
unix {
    DEFINES += _UNIX_SYSTEM_
    LIBSPATH =

    SOURCES += \
        $$PWD/private/unix/keyboard_p.cpp \
        $$PWD/private/unix/mouse_p.cpp \
        $$PWD/private/unix/command_p.cpp \
        $$PWD/private/unix/volume_p.cpp

    HEADERS  += \
        $$PWD/private/unix/keyboard_p.h \
        $$PWD/private/unix/mouse_p.h \
        $$PWD/private/unix/command_p.h \
        $$PWD/private/unix/volume_p.h

    LIBS +=
}
SOURCES += \
    $$PWD/system_keyboard.cpp \
    $$PWD/system_mouse.cpp \
    $$PWD/system_command.cpp \
    $$PWD/system_volume.cpp \
    $$PWD/system_type.cpp \
    $$PWD/command_execution.cpp


HEADERS  += \
    $$PWD/system_keyboard.h \
    $$PWD/system_mouse.h \
    $$PWD/system_volume.h \
    $$PWD/system_command.h \
    $$PWD/system_type.h \
    $$PWD/command_execution.h \
    $$PWD/private/system_keyboard_p.h \
    $$PWD/private/system_mouse_p.h \
    $$PWD/private/system_command_p.h \
    $$PWD/private/system_volume_p.h \
    $$PWD/private/system_type_p.h

