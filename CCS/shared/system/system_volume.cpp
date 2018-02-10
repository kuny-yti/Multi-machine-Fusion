#include "system_volume.h"

#ifdef _WINDOWS_SYSTEM_
#include "private/windows/xp_volume_p.h"
#include "private/windows/win7_volume_p.h"
#elif _UNIX_SYSTEM_
#include "private/unix/mouse_p.h"
#else
#error On the function of the system implementation is not defined.
#endif

system_volume::~system_volume()
{
    delete d_ptr;
}
system_volume::system_volume(const QString &cmd, const uint val)
{
    os_tyep = new system_type();

    switch (os_tyep->os_type())
    {
    case WindowsVista :
    case Windows7 :
    case Windows8 :
        d_ptr = new win7_volume_p();
        break;
    case WindowsXP:
        d_ptr = new xp_volume_p();
        break;
    default:
        break;
    }

    cmd_str = cmd;
    cmd_val = val;
}
system_volume::system_volume()
{
    d_ptr = new xp_volume_p();
    cmd_val = 0;
}
QString &system_volume::cmd_table()
{
    return cmd_str;
}
uint &system_volume::cmd_table_value()
{
    return cmd_val;
}
void system_volume::setVolume(const ushort val)
{
    d_ptr->setVolume(val);
}
ushort system_volume::volume()
{
    return d_ptr->volume();
}

void system_volume::setMute(const bool val)
{
    d_ptr->setMute(val);
}
bool system_volume::mute()
{
    return d_ptr->mute();
}
