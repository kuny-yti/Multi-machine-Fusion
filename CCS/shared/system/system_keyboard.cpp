#include "system_keyboard.h"

#ifdef _WINDOWS_SYSTEM_
#include "private/windows/keyboard_p.h"
#elif _UNIX_SYSTEM_
#include "private/unix/keyboard_p.h"
#else
#error On the function of the system implementation is not defined.
#endif
system_keyboard::system_keyboard(const QString &cmd, const uint val)
{
    d_ptr = new keyboard_p();

    cmd_str = cmd;
    cmd_val = val;
}
system_keyboard::system_keyboard()
{
    d_ptr = new keyboard_p();

    cmd_val = 0;
}
system_keyboard::~system_keyboard()
{
    delete d_ptr;
}
QString &system_keyboard::cmd_table()
{
    return cmd_str;
}
uint &system_keyboard::cmd_table_value()
{
    return cmd_val;
}

QByteArray system_keyboard::toByte(const QKeyEvent &key)
{
    return d_ptr->toByte(key);
}
QKeyEvent system_keyboard::toEvent(const QByteArray &byte)
{
    return d_ptr->toEvent(byte);
}
void system_keyboard::excEvent(const QKeyEvent&ke)
{
    return d_ptr->excEvent(ke);
}
void system_keyboard::excEvent(const QByteArray&by)
{
    return d_ptr->excEvent(by);
}
