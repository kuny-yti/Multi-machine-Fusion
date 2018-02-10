#ifndef TYPE_P_H
#define TYPE_P_H
#include "private/system_type_p.h"
class type_p : public system_type_p
{
    bool isWOW64() const;
public:
    type_p();

    virtual OSType os_type();
    virtual PlatformType platform_type();
};

#endif // TYPE_P_H
