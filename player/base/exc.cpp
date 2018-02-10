
#include <cstring>
#include <cerrno>

#include "gettext.h"
#define _(string) gettext(string)

#include "msg.h"
#include "exc.h"


const char *exc::_fallback_str = strerror(ENOMEM);

exc::exc() throw ()
    : _fallback(false), _str(), _sys_errno(0)
{
}

exc::exc(const std::string &what, int sys_errno) throw ()
    : _fallback(false), _sys_errno(sys_errno)
{
    try
    {
        _str = what;
    }
    catch (...)
    {
        _fallback = true;
        _sys_errno = ENOMEM;
    }
    if (!empty())
    {
        msg::dbg_txt(_("Exception: %s"), _str.c_str());
    }
}

exc::exc(int sys_errno) throw ()
    : _fallback(false), _sys_errno(sys_errno)
{
    try
    {
        _str = strerror(_sys_errno);
    }
    catch (...)
    {
        _fallback = true;
        _sys_errno = ENOMEM;
    }
    if (!empty())
    {
        msg::dbg_txt(_("Exception: %s"), _str.c_str());
    }
}

exc::exc(const exc &e) throw ()
    : _fallback(e._fallback), _sys_errno(e._sys_errno)
{
    try
    {
        _str = e._str;
    }
    catch (...)
    {
        _fallback = true;
        _sys_errno = ENOMEM;
    }
}

exc::exc(const std::exception &e) throw ()
    : _fallback(false), _sys_errno(0)
{
    // TODO: Avoid the crappy what() strings; ideally translate them to errnos.
    // E.g. std::bad_alloc -> ENOMEM.
    try
    {
        _str = e.what();
    }
    catch (...)
    {
        _fallback = true;
        _sys_errno = ENOMEM;
    }
    if (!empty())
    {
        msg::dbg_txt(_("Exception: %s"), _str.c_str());
    }
}

exc::~exc() throw ()
{
}

bool exc::empty() const throw ()
{
    return (_str.length() == 0 && _sys_errno == 0 && !_fallback);
}

int exc::sys_errno() const throw ()
{
    return _sys_errno;
}

const char *exc::what() const throw ()
{
    return (_fallback ? _fallback_str : _str.c_str());
}
