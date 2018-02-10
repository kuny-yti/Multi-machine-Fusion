#include "system_type.h"

#ifdef _WINDOWS_SYSTEM_
#include "private/windows/type_p.h"
#elif _UNIX_SYSTEM_
#include "private/unix/type_p.h"
#else
#error On the function of the system implementation is not defined.
#endif

system_type::system_type()
{
    d_ptr = new type_p();
}
system_type::~system_type()
{
    delete d_ptr;
}
OSType system_type::os_type()
{
    return d_ptr->os_type();
}

PlatformType system_type::platform_type()
{
    return d_ptr->platform_type();
}
