#include "command_execution.h"
#include "message.h"
#include "tcp_interfaces.h"
#include <QTime>

void command_execution::_set_cmd(const QString &str, const QByteArray &byte)
{
    if (str == "Shutdown")
    {
        message::warning(QObject::tr("Shutdown pc.  [time:%1")
                         .arg(QTime::currentTime ().toString ("hh:mm:ss")));

        command->shutdown();
    }
    else if (str == "Boot")
    {
        message::warning(QObject::tr("Boot pc.  [time:%1")
                         .arg(QTime::currentTime ().toString ("hh:mm:ss")));
    }
    else if (str == "Restart")
    {
        message::warning(QObject::tr("reboot pc.  [time:%1")
                         .arg(QTime::currentTime ().toString ("hh:mm:ss")));
        command->restart();
    }
    else if (str == "KeyboardMouse")
    {
        message::information(QObject::tr("keyboard key(%1) [time:%2]").arg(keyboard->toEvent(byte).text())
                         .arg(QTime::currentTime().toString("hh:mm:ss")));
        keyboard->excEvent(byte);
        mouse->excEvent(byte);
    }
    else if (str == "Volume")
    {
        volume->setVolume(byte.toFloat());
    }
}
command_execution::command_execution(configure *cf)
{
    conf = cf;
    command = new system_command();
    keyboard= new system_keyboard();
    mouse= new system_mouse();
    volume= new system_volume();
}

bool command_execution::execution(const QString &cmd, const QByteArray &byte)
{
    QStringList tem ;
    tem += conf->command();

    for (int i = 0; i < tem.size(); i++)
    {
        if (tem[i] == cmd)
        {
            QStringList tems = conf->command_get(cmd);
            _set_cmd(tems[0], byte);
            return true;
        }
    }
    return false;
}
bool command_execution::execution(const QByteArray &byte)
{
    QByteArray data = byte;
    QString cmd = toCmd(data);
    return execution(cmd, data);
}
bool command_execution::execution(const uint cmd, const QByteArray &byte)
{
    switch (cmd)
    {
    case Cmd_System_Shutdown://由控制端发出
        message::warning(QObject::tr("Shutdown pc.  [time:%1")
                         .arg(QTime::currentTime ().toString ("hh:mm:ss")));

        command->shutdown();
        break;
    case Cmd_System_Boot:
        message::warning(QObject::tr("Boot pc.  [time:%1")
                         .arg(QTime::currentTime ().toString ("hh:mm:ss")));
        break;
    case Cmd_System_Volume:
        volume->setVolume(byte.toFloat());
        break;
    case Cmd_System_KeyboardMouse:
        message::information(QObject::tr("keyboard key(%1) [time:%2]").
                             arg(keyboard->toEvent(byte).text())
                         .arg(QTime::currentTime().toString("hh:mm:ss")));
        keyboard->excEvent(byte);
        mouse->excEvent(byte);
        break;
    case Cmd_System_Restart:
        message::warning(QObject::tr("reboot pc.  [time:%1")
                         .arg(QTime::currentTime ().toString ("hh:mm:ss")));
        command->restart();
        break;
    case Cmd_Softwrare_Off://由控制端发出
    case Cmd_Softwrare_On:
        //软件执行
        break;
        default:
            break;
    }
}

QByteArray command_execution::toByte(const QString &cmd, const QByteArray &byte)
{
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_1);

    out << cmd << byte;
    return data;
}
QString command_execution::toCmd(QByteArray &byte)
{
    QString cmd;
    QByteArray data = byte;
    QDataStream out(&data, QIODevice::ReadOnly);
    out.setVersion(QDataStream::Qt_5_1);
    out >> cmd >> byte;
    return cmd;
}
