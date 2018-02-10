#-------------------------------------------------
#
# Project created by QtCreator 2014-01-19T09:40:48
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = server
TEMPLATE = app

DEFINES += CPU_X86

OTHER_FILES +=
RC_FILE +=

RESOURCES +=

SOURCES += \
    cpu_capacity.cpp \
    main.cpp \
    ccpuid.cpp \
    memcpy_cpu.cpp

HEADERS += \
    cpu_capacity.h \
    ccpuid.h \
    ccpuid.hpp \
    memcpy_cpu.h
