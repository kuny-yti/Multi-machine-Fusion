#include "command_p.h"
#include <windows.h>

command_p::command_p()
{
}
command_p::~command_p()
{

}
void command_p::_sys_cmd(uint code)
{
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
        return ;
    LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);

    tkp.PrivilegeCount = 1; // one privilege to set
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
    if (GetLastError() != ERROR_SUCCESS)
        return;

    ExitWindowsEx(code | EWX_FORCE, 0L);
}
void command_p::shutdown()
{
    _sys_cmd(EWX_SHUTDOWN);
}
void command_p::restart()
{
    _sys_cmd(EWX_REBOOT);
}
void command_p::logout()
{
    _sys_cmd(EWX_LOGOFF);
}
