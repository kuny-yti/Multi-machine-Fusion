
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <new>
#if HAVE_BACKTRACE
# include <cstdio>
# include <execinfo.h>
#endif
#if __GNUG__
# include <cxxabi.h>
# include <cstring>
#endif
#if HAVE_SIGACTION
# include <signal.h>
#endif

#include "gettext.h"
#define _(string) gettext(string)

#include "msg.h"
#include "dbg.h"


namespace dbg
{
#if HAVE_SIGACTION
    static void signal_crash(int signum)
    {
        msg::err(_("Caught signal %d (%s). Aborting."), signum, strsignal(signum));
        crash();
    }
#endif

    static void exception_crash()
    {
        msg::err(_("Unexpected exception."));
        crash();
    }

    void oom_abort()
    {
        msg::err("%s", strerror(ENOMEM));
        abort();
    }

    void init_crashhandler()
    {
#if HAVE_SIGACTION
        struct sigaction signal_handler;
        signal_handler.sa_handler = signal_crash;
        sigemptyset(&signal_handler.sa_mask);
        signal_handler.sa_flags = 0;
        (void)sigaction(SIGILL, &signal_handler, NULL);
        (void)sigaction(SIGFPE, &signal_handler, NULL);
        (void)sigaction(SIGSEGV, &signal_handler, NULL);
#endif
        std::set_unexpected(exception_crash);
        std::set_terminate(exception_crash);
        std::set_new_handler(oom_abort);
    }

#ifndef NDEBUG
# if HAVE_BACKTRACE
    static char *get_file_and_line(
            const char *executable, size_t executable_len,
            const char *addr, size_t addr_len)
    {
        /* "addr2line -e EXECUTABLE ADDR" */
        size_t cmd_len = 9 + 4 + executable_len + 1 + addr_len;
        char *cmd_buf = static_cast<char*>(malloc(cmd_len + 1));
        if (!cmd_buf)
        {
            return NULL;
        }
        strcpy(cmd_buf, "addr2line -e ");
        strncat(cmd_buf, executable, executable_len);
        strcat(cmd_buf, " ");
        strncat(cmd_buf, addr, addr_len);
        FILE *f = popen(cmd_buf, "r");
        free(cmd_buf);
        if (!f)
        {
            return NULL;
        }
        char line_buf[512];
        if (!fgets(line_buf, 512, f))
        {
            pclose(f);
            return NULL;
        }
        if (pclose(f) != 0)
        {
            return NULL;
        }
        char *p = strchr(line_buf, '\n');
        if (p)
        {
            *p = '\0';
        }
        if (strcmp(line_buf, "??:0") == 0)
        {
            return NULL;
        }
        return strdup(line_buf);
    }
# endif
#endif

    void backtrace()
    {
#ifndef NDEBUG
# if HAVE_BACKTRACE
        // Adapted from the example in the glibc manual
        void *array[64];
        int size;
        char **strings;

        size = ::backtrace(array, 64);

        if (size == 0)
        {
            msg::err("No backtrace available.");
        }
        else
        {
            msg::err("Backtrace:");
            strings = backtrace_symbols(array, size);
            for (int i = 0; i < size; i++)
            {
                char *p;
                char *executable = NULL;
                size_t executable_len = 0;
                char *addr = NULL;
                size_t addr_len = 0;
                executable = strings[i];
                p = strchr(strings[i], '(');
                if (p)
                {
                    executable_len = p - strings[i];
                }
                p = strchr(strings[i], '[');
                if (p)
                {
                    addr = p + 1;
                    p = strchr(addr, ']');
                    if (p)
                    {
                        addr_len = p - addr;
                    }
                }
                char *file_and_line = NULL;
                if (executable && executable_len > 0 && addr && addr_len > 0)
                {
                    file_and_line = get_file_and_line(executable, executable_len, addr, addr_len);
                }
#  if __GNUG__
                int status;
                char *q;
                char *realname = NULL;
                p = strchr(strings[i], '(');
                if (p)
                {
                    p++;
                    q = strchr(p, '+');
                    if (q)
                    {
                        *q = '\0';
                        realname = abi::__cxa_demangle(p, NULL, NULL, &status);
                        *q = '+';
                    }
                }
                if (realname && status == 0)
                {
                    *p = '\0';
                    msg::err(4, "%s%s%s", strings[i], realname, q);
                }
                else
                {
                    msg::err(4, "%s", strings[i]);
                }
                free(realname);
#  else
                msg::err(4, "%s", strings[i]);
#  endif
                if (file_and_line)
                {
                    msg::err(8, "at %s", file_and_line);
                    free(file_and_line);
                }
            }
            free(strings);
        }
# endif
#endif
    }

    void crash()
    {
        backtrace();
        /* TRANSLATORS: %s will be a mail address. */
        msg::err(_("Report bugs to <%s>."), "PACKAGE_BUGREPORT");
        abort();
    }
}
