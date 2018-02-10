#ifndef SYSTEM_TYPE_P_H
#define SYSTEM_TYPE_P_H

#include <QtGlobal>

typedef enum //枚举操作系统类型
{
    UnknownOS = 0,
    WindowsXP ,
    WindowsVista ,
    WindowsServer2008 ,
    Windows7 ,
    Windows8 ,
    WindowsServer2008R2,
    WindowsServer2003,
    WindowsServer2000,
    WindowsNT,
    Windows95OSR2,
    Windows95,
    Windows98SE,
    Windows98,
    WindowsME,
    WindowsUnknown95family
}OSType;

typedef enum
{
    UnknownPlatform,
    x86_WOW64,
    x86,
    x64
}PlatformType;

class system_type_p
{
public:
    virtual ~system_type_p(){}

    virtual OSType os_type() = 0;
    virtual PlatformType platform_type() = 0;
};

#endif // SYSTEM_TYPE_P_H
