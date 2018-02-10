
#include <cerrno>
#ifdef HAVE_CLOCK_GETTIME
# include <time.h>
#else
# include <sys/time.h>
#endif

#include "gettext.h"
#define _(string) gettext(string)

#include "exc.h"
#include "timer_.h"


int64_t timer_::get_microseconds(timer_::type t)
{
#ifdef HAVE_CLOCK_GETTIME

    struct timespec time;
    int r;
    if (t == realtime) {
        r = clock_gettime(CLOCK_REALTIME, &time);
    } else if (t == monotonic) {
        r = clock_gettime(CLOCK_MONOTONIC, &time);
    } else if (t == process_cpu) {
#if defined(CLOCK_PROCESS_CPUTIME_ID)
        r = clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time);
#elif defined(CLOCK_PROF)
        r = clock_gettime(CLOCK_PROF, &time);
#else
        r = -1;
        errno = ENOSYS;
#endif
    } else /* t == thread_cpu */ {
#ifdef CLOCK_THREAD_CPUTIME_ID
        r = clock_gettime(CLOCK_THREAD_CPUTIME_ID, &time);
#else
        r = -1;
        errno = ENOSYS;
#endif
    }
    if (r != 0)
        throw exc(_("Cannot get time."), errno);
    return static_cast<int64_t>(time.tv_sec) * 1000000 + time.tv_nsec / 1000;

#else

    if (t == realtime || t == monotonic) {
        struct timeval tv;
        int r = gettimeofday(&tv, NULL);
        if (r != 0)
            throw exc(_("Cannot get time."), errno);
        return static_cast<int64_t>(tv.tv_sec) * 1000000 + tv.tv_usec;
    } else if (t == process_cpu) {
        // In W32, clock() starts with zero on program start, so we do not need
        // to subtract a start value.
        // XXX: Is this also true on Mac OS X?
        clock_t c = clock();
        return (c * static_cast<int64_t>(1000000) / CLOCKS_PER_SEC);
    } else {
        throw exc(_("Cannot get time."), ENOSYS);
    }

#endif
}
