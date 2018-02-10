#ifndef _CTRL_H
#define _CTRL_H

#include <QByteArray>
#include <QVector>
#include "ui_define.h"

#define wParameBegin() parameter().clear(); QDataStream write(&parameter(), QIODevice::ReadWrite);
#define wParame write
#define wParameEnd()

#define rParameBegin() QDataStream read(&parameter(), QIODevice::ReadWrite);
#define rParame read
#define rParameEnd()

#define controls(cmd) ctrl_core::control(cmd)

class ctrl
{
public:
    ctrl();
    ~ctrl();
    //控制命令
    static void control(const int& cmd);
    static QByteArray &parameter();
    static RawConfig &raw_config();

    static QByteArray serialize();
    static bool unserialize(const QByteArray &in);

    //继承后可以接受来自控制中心的命令
    virtual void command(const int& cmd) = 0;
};

//! 整个程序控制中心。
class ctrl_core
{
    QVector <ctrl*> _ctrl;
    QByteArray _parameters;
    RawConfig _raw_config;
    static ctrl_core *_this;

public:
    explicit ctrl_core();
    virtual ~ctrl_core();

    //返回控制中心
    static ctrl_core* core();
    static QByteArray &parameter();
    static RawConfig &raw_config();
    //控制命令
    static void control(const int& cmd);

    //注册一个控制对象
    static void registers(ctrl* c);
    //注销一个控制对象
    static void deregisters(ctrl* c);
};
#endif
