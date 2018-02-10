#include "type_p.h"
#include <windows.h>
#include <QString>

type_p::type_p()
{
}
static OSType _os(QString &version)
{
#if WINVER >= _WIN32_WINNT_WIN2K
    OSVERSIONINFOEX info;
#else
    OSVERSIONINFO info;
#endif

    info.dwOSVersionInfoSize = sizeof(info);
    if (GetVersionEx((OSVERSIONINFO*) &info))
    {
        switch (info.dwPlatformId)
        {
        case VER_PLATFORM_WIN32_NT:
            #if WINVER >= _WIN32_WINNT_WIN2K
            if (info.dwMajorVersion == 6)
            {
                if(info.dwMinorVersion == 0)
                {
                    if (info.wProductType == VER_NT_WORKSTATION)
                    {
                        return WindowsVista;
                    }
                    else
                    {
                        return WindowsServer2008;
                    }
                }
                else if(info.dwMinorVersion == 1)
                {
                    if (info.wProductType == VER_NT_WORKSTATION)
                    {
                        return Windows7;
                    }
                    else
                    {
                        return WindowsServer2008R2;
                    }
                }
            }
            #endif

            if (info.dwMajorVersion == 5 && info.dwMinorVersion == 2)
            {
                return WindowsServer2003;
            }
            if (info.dwMajorVersion == 5 && info.dwMinorVersion == 1)
            {
                return WindowsXP;
            }
            if (info.dwMajorVersion == 5 && info.dwMinorVersion == 0)
            {
                return WindowsServer2000;
            }
            if (info.dwMajorVersion <= 4)
            {
                return WindowsNT;
            }
            version = QString("Microsoft Windows %1.%2").arg(info.dwMajorVersion)
                    .arg(info.dwMinorVersion);
            return UnknownOS;

        case VER_PLATFORM_WIN32_WINDOWS:
            if (info.dwMajorVersion == 4 && info.dwMinorVersion == 0)
            {
                if (info.szCSDVersion[1] == 'C' ||
                    info.szCSDVersion[1] == 'B')
                {
                    return Windows95OSR2;
                }
                return Windows95;
            }
            if (info.dwMajorVersion == 4 && info.dwMinorVersion == 10)
            {
                if (info.szCSDVersion[1] == 'A')
                {
                    return Windows98SE;
                }
                return Windows98;
            }
            if (info.dwMajorVersion == 4 && info.dwMinorVersion == 90)
            {
                return WindowsME;
            }
            if (info.dwMajorVersion == 4)
            {
                return WindowsUnknown95family;
            }
            break;

        default:
            break;
        }
    }
    return UnknownOS;
}
OSType type_p::os_type()
{
    QString version;
    return _os(version);
}
PlatformType type_p::platform_type()
{
#ifdef _X86_
    if(isWOW64())
        return x86_WOW64;
    else
        return x86;
#else
#ifdef _AMD64_
    return x64;
#else
    return UnknownPlatform;
#endif
#endif
}

bool type_p::isWOW64() const
{
#if WINVER >= _WIN32_WINNT_WINXP
    typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
    HMODULE hModule = GetModuleHandleA("kernel32");
    if (!hModule) return FALSE;

    LPFN_ISWOW64PROCESS fnIsWow64Process =
        (LPFN_ISWOW64PROCESS) GetProcAddress(hModule, "IsWow64Process");

    BOOL bIsWow64 = FALSE;
    if(NULL != fnIsWow64Process &&
        fnIsWow64Process(GetCurrentProcess(), &bIsWow64) &&
        bIsWow64)
    {
        return true;
    }
#endif
    return false;
}
