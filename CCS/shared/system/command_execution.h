#ifndef COMMAND_EXECUTION_H
#define COMMAND_EXECUTION_H
#include <QString>
#include "configure.h"

#include "system_command.h"
#include "system_keyboard.h"
#include "system_mouse.h"
#include"system_volume.h"

class command_execution
{
    configure *conf;
    system_command *command;
    system_keyboard *keyboard;
    system_mouse *mouse;
    system_volume *volume;
    void _set_cmd(const QString &str, const QByteArray &byte);
public:
    command_execution(configure *);

    bool execution(const QString &cmd, const QByteArray &byte);
    bool execution(const QByteArray &byte);
    bool execution(const uint cmd, const QByteArray &byte);

    QByteArray toByte(const QString &cmd, const QByteArray &byte);
    QString toCmd(QByteArray &byte);
};

#endif // COMMAND_EXECUTION_H
