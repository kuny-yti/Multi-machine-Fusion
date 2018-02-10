#-------------------------------------------------
#
# Project created by QtCreator 2014-01-19T09:40:48
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = server
TEMPLATE = app

DEFINES += _MESSAFE_LOGS_FILE_

include(../shared/bulid/bulid.pri)
include(../shared/tcp_server/tcp_server.pri)
include(./server_pro/server_pro.pri)
include(./server_ui/server_ui.pri)
include(../shared/tcp_comm/tcp_comm.pri)
include(../shared/lib/lib.pri)
include(../shared/system/system.pri)
include(../shared/message/message.pri)
include(../shared/configure/configure.pri)
include(../shared/time/time.pri)

OTHER_FILES += \
    resource.rc
RC_FILE += resource.rc

RESOURCES += \
    ../shared/res/resource.qrc

SOURCES += main.cpp
