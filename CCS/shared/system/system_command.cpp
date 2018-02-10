#include "system_command.h"

#ifdef _WINDOWS_SYSTEM_
#include "private/windows/command_p.h"
#elif _UNIX_SYSTEM_
#include "private/unix/command_p.h"
#else
#error On the function of the system implementation is not defined.
#endif

system_command::system_command(const QString &cmd, const uint val)
{
    d_ptr = new command_p();

    cmd_str = cmd;
    cmd_val = val;
}
system_command::system_command()
{
    d_ptr = new command_p();

    cmd_val = 0;
}
system_command::~system_command()
{
    delete d_ptr;
}
QString &system_command::cmd_table()
{
    return cmd_str;
}
uint &system_command::cmd_table_value()
{
    return cmd_val;
}
void system_command::shutdown()
{
    d_ptr->shutdown();
}
void system_command::restart()
{
    d_ptr->restart();
}
void system_command::logout()
{
    d_ptr->logout();
}
