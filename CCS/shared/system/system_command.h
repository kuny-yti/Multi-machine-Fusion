#ifndef SYSTEM_COMMAND_H
#define SYSTEM_COMMAND_H

#include <QtGlobal>
#include <QString>

class system_command_p;
class system_command
{
    system_command_p *d_ptr;
    QString cmd_str;
    uint cmd_val;

public:
    system_command();
    ~system_command();
    system_command(const QString &cmd, const uint val);

    QString &cmd_table();
    uint &cmd_table_value();

    void shutdown();
    void restart();
    void logout();
};

#endif // SYSTEM_COMMAND_H
