#-------------------------------------------------
#
# Project created by QtCreator 2014-02-18T08:50:57
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Fusion_v1.3.1
TEMPLATE = app

#include(./player/player.pri)
include(./render_api/render_api.pri)
include(./glsl_image/glsl_image.pri)
include(./surface/surface.pri)
include(./viewer/viewer.pri)
include(./ui/ui.pri)
include(./desktop/desktop.pri)


DEFINES += _MESSAFE_LOGS_FILE_

include(CCS/shared/tcp_comm/tcp_comm.pri)
include(CCS/shared/tcp_client/tcp_client.pri)
include(CCS/shared/system/system.pri)
include(CCS/shared/message/message.pri)
include(CCS/shared/configure/configure.pri)
include(CCS/client/client_pro/client_pro.pri)
#include(./license/license.pri)


TRANSLATIONS = $${PWD}/language/language_zh_CN.ts \
               $${PWD}/language/language_en.ts

DESTDIR = $${PWD}/bin
UI_DIR = $${PWD}/bin/temp/.ui
MOC_DIR = $${PWD}/bin/temp/.moc
OBJECTS_DIR = $${PWD}/bin/temp/.obj
RCC_DIR  = $${PWD}/bin/temp/.rcc

SOURCES += main.cpp \
    main_dispatch.cpp

HEADERS  += \
    main_dispatch.h

FORMS    +=

RC_FILE +=  \
    resource.rc

OTHER_FILES += \
    resource.rc
