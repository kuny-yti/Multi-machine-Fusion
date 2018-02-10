#ifndef SYSTEM_TYPE_H
#define SYSTEM_TYPE_H

#include "private/system_type_p.h"

class system_type
{
    system_type_p *d_ptr;
public:
    system_type();
    ~system_type();

    OSType os_type();
    PlatformType platform_type();
};

#endif // SYSTEM_TYPE_H
