#include "system_mouse.h"

#ifdef _WINDOWS_SYSTEM_
#include "private/windows/mouse_p.h"
#elif _UNIX_SYSTEM_
#include "private/unix/mouse_p.h"
#else
#error On the function of the system implementation is not defined.
#endif

system_mouse::system_mouse(const QString &cmd, const uint val)
{
    d_ptr = new mouse_p();

    cmd_str = cmd;
    cmd_val = val;
}
system_mouse::system_mouse()
{
    d_ptr = new mouse_p();

    cmd_val = 0;
}
system_mouse::~system_mouse()
{
    delete d_ptr;
}
QString &system_mouse::cmd_table()
{
    return cmd_str;
}
uint &system_mouse::cmd_table_value()
{
    return cmd_val;
}
QByteArray system_mouse::toByte(bool v, const QMouseEvent &mu, const QWheelEvent &wh)
{
    return d_ptr->toByte(v,mu, wh);
}
QMouseEvent system_mouse::toEventMouse(const QByteArray &byte)
{
    return d_ptr->toEventMouse(byte);
}
QWheelEvent system_mouse::toEventWheel(const QByteArray &byte)
{
    return d_ptr->toEventWheel(byte);
}
void system_mouse::excEvent(const QMouseEvent&byte)
{
    return d_ptr->excEvent(byte);
}
void system_mouse::excEvent(const QWheelEvent&byte)
{
    return d_ptr->excEvent(byte);
}
void system_mouse::excEvent(const QByteArray&byte)
{
    return d_ptr->excEvent(byte);
}
