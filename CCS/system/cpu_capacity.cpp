#include "cpu_capacity.h"
#include "ccpuid.h"
#include <QDebug>

#if defined(_WIN32)
#include <pthread.h>
#elif defined(__APPLE__) || defined(BSD_FAMILY) && !defined(__HAIKU__)
#include <sys/types.h>
#include <sys/sysctl.h>
#else
#include <string.h>
#include <sched.h>
#endif

quint32 cpu_capacity::_cpu_cap = 0;
quint32 cpu_capacity::_cpu_mask = 0xffffffff;
#define _cpuid(a,b,c,d,e) __cpuid(a,b,c,d,e)

cpu_capacity::cpu_capacity()
{
}

void cpu_capacity::init( void)
{
    qDebug("[cpu_capacity]Checking CPU capacity.");
    _cpu_cap = 0;
    _cpu_mask = 0xffffffff;

    #ifdef CPU_X86
    int eax, ebx, ecx, edx;
    int max_std_level, max_ext_level;

    #if !defined(CPU_64BIT) // 64 bits CPU have all cpuid
    long a, c;

    __asm__ __volatile__ (
                     "pushf\n\t"
                     "pop %0\n\t"
                     "mov %0, %1\n\t"

                     "xor $0x200000, %0\n\t"
                     "push %0\n\t"
                     "popf\n\t"

                     "pushf\n\t"
                     "pop %0\n\t"
                     : "=a" (a), "=c" (c)
                     :
                     : "cc"
                     );

    if (a == c)
    return ;
    #endif

    _cpuid(0, max_std_level, ebx, ecx, edx);

    if(max_std_level >= 1)
    {
        int std_caps = 0;

        _cpuid(1, eax, ebx, ecx, std_caps);
        if (std_caps & (1<<23))
            _cpu_cap |= CPU_CAPACITY_MMX;
        if (std_caps & (1<<25))
            _cpu_cap |= CPU_CAPACITY_MMXEXT | CPU_CAPACITY_SSE;
        if (std_caps & (1<<26))
            _cpu_cap |= CPU_CAPACITY_SSE2;
        if (ecx & 1)
            _cpu_cap |= CPU_CAPACITY_SSE3;
        if (ecx & 0x00000200 )
            _cpu_cap |= CPU_CAPACITY_SSSE3;

    }

    _cpuid(0x80000000, max_ext_level, ebx, ecx, edx);

    if(max_ext_level >= 0x80000001)
    {
        int ext_caps = 0;

        _cpuid(0x80000001, eax, ebx, ecx, ext_caps);
        if (ext_caps & (1<<31))
           _cpu_cap |= CPU_CAPACITY_3DNOW;
        if (ext_caps & (1<<30))
           _cpu_cap |= CPU_CAPACITY_3DNOWEXT;
        if (ext_caps & (1<<23))
           _cpu_cap |= CPU_CAPACITY_MMX;
        if (ext_caps & (1<<22))
           _cpu_cap |= CPU_CAPACITY_MMXEXT;

    }

    #define CHECK(x) if(_cpu_cap & CPU_CAPACITY_##x)  \
    { \
        qDebug("\t\t"#x" detected ");\
        if(!(_cpu_mask & CPU_CAPACITY_##x)) \
            qDebug("  but disabled");\
        qDebug("\n");\
    }

    CHECK(MMX);
    CHECK(3DNOW);
    CHECK(3DNOWEXT);
    CHECK(MMXEXT);
    CHECK(SSE);
    CHECK(SSE2);
    CHECK(SSE3);
    CHECK(SSSE3);

    #endif // X86
    qDebug("[cpu_capacity]End of CPU capacity check (cpuMask :%x, cpuCaps :%x)\n",
           _cpu_cap, _cpu_mask);
    return ;
}

int cpu_capacity::cpu_processors_count(void)
{
#if defined(_WIN32)
    return pthread_num_processors_np();
#elif defined(__APPLE__) || defined(ADM_BSD_FAMILY) && !defined(__HAIKU__)
    int np;

    size_t length = sizeof(np);

    if (sysctlbyname("hw.ncpu", &np, &length, NULL, 0))
        np = 1;

    return np;
#elif defined(__sun__)
    return sysconf( _SC_NPROCESSORS_ONLN );
#elif defined(__unix__) && !defined(__CYGWIN__)
    unsigned int bit;
    int np;

    cpu_set_t p_aff;
    memset( &p_aff, 0, sizeof(p_aff) );
    sched_getaffinity( 0, sizeof(p_aff), &p_aff );

    for( np = 0, bit = 0; bit < sizeof(p_aff); bit++ )
        np += (((uint8_t *)&p_aff)[bit / 8] >> (bit % 8)) & 1;

    return np;
#else
    return 1;
#endif
}
