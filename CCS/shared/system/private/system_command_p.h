#ifndef SYSTEM_COMMAND_P_H
#define SYSTEM_COMMAND_P_H

class system_command_p
{
public:
    virtual ~system_command_p(){}

    virtual void shutdown() = 0;
    virtual void restart() = 0;
    virtual void logout() = 0;
};

#endif // COMMAND_H
