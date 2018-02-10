#-------------------------------------------------
#
# Project created by QtCreator 2013-11-16T02:11:36
#
# Expected to support:
# 1.TCP Server
# 2.UDP Server
# 3.SerialPort
# 4.WEB Server
# 5.Desktop Server
#
# Complete support
# 1.TCP Server(server, client)
# 2.WEB Server(http 1.1, html)
#-------------------------------------------------

TEMPLATE = subdirs

SUBDIRS += ./server
#SUBDIRS += ./client
#SUBDIRS += ./clientCS

CONFIG += ordered
