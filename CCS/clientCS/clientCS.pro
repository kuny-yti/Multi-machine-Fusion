#-------------------------------------------------
#
# Project created by QtCreator 2014-01-21T15:19:55
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = clientCS
TEMPLATE = app

DEFINES += _MESSAFE_LOGS_FILE_ #开启系统日志记录

include(../shared/bulid/bulid.pri)
include(../shared/lib/lib.pri)              #转码、随机数、sha1加解密
include(../shared/tcp_comm/tcp_comm.pri)    #tcp cs架构通讯机制实现
include(../shared/tcp_client/tcp_client.pri)#tcp cs架构客户端实现
include(../shared/system/system.pri)        #键盘、鼠标、音量、重启、关机、注销的控制和模拟
include(../shared/message/message.pri)      #实现自定义日志的写入
include(../shared/configure/configure.pri)  #负责tcp cs架构的命令表和软件的配置
include(./client_ui/client_ui.pri)  #本地软件界面
include(./client_pro/client_pro.pri)
include(./key_mouse_ui/key_mouse_ui.pri)
#include(./license/license.pri)      #软件许可证和时间限制

OTHER_FILES += \
    resource.rc
RC_FILE += resource.rc

RESOURCES += \
    ../shared/res/resource.qrc

SOURCES += main.cpp


