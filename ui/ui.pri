INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

DEFINES += CheckSoftware

FORMS += \
    $$PWD/tools_gui.ui \
    $$PWD/debug_gui.ui \
    $$PWD/color_gui.ui \
    $$PWD/computer_gui.ui

HEADERS += \
    $$PWD/tools_gui.h \
    $$PWD/debug_gui.h \
    $$PWD/ui_define.h \
    $$PWD/computer_area_gui.h \
    $$PWD/device_area_gui.h \
    $$PWD/ctrl.h \
    $$PWD/intersect_test.h \
    $$PWD/opengl_viewer.h \
    $$PWD/fusion_calc.h \
    $$PWD/system_config.h \
    $$PWD/gui_config.h \
    $$PWD/color_gui.h \
    $$PWD/computer_gui.h \
    $$PWD/direct_show.h 

SOURCES += \
    $$PWD/tools_gui.cpp \
    $$PWD/debug_gui.cpp \
    $$PWD/computer_area_gui.cpp \
    $$PWD/device_area_gui.cpp \
    $$PWD/ctrl.cpp \
    $$PWD/intersect_test.cpp \
    $$PWD/opengl_viewer.cpp \
    $$PWD/fusion_calc.cpp \
    $$PWD/system_config.cpp \
    $$PWD/color_gui.cpp \
    $$PWD/computer_gui.cpp \
    $$PWD/direct_show.cpp

RESOURCES += \
    $$PWD/Resources/resources.qrc
LIBS+=  -lole32 -lstrmiids
