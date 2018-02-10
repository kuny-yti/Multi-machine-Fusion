#ifndef COMMAND_P_H
#define COMMAND_P_H

#include "private/system_command_p.h"
#include <QtGlobal>

class command_p :public system_command_p
{
    void _sys_cmd(uint code);
public:
    command_p();
    ~command_p();

    virtual void shutdown();
    virtual void restart();
    virtual void logout();
};

#endif // COMMAND_P_H
