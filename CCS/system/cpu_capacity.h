#ifndef CPU_SUPPORT_H
#define CPU_SUPPORT_H

#include <QtGlobal>

#ifdef CPU_X86

#define CHECK_Z(x) {if(cpu_capacity::_cpu_cap & CPU_CAPACITY_##x & cpu_capacity::_cpu_mask) return 1; else return 0;}

#else
#define         CHECK_Z(x) {return 0;}
#endif

typedef enum
{
    CPU_CAPACITY_NONE   =1,
    CPU_CAPACITY_MMX    =1<<1,
    CPU_CAPACITY_MMXEXT =1<<2,
    CPU_CAPACITY_3DNOW  =1<<3,
    CPU_CAPACITY_3DNOWEXT  =1<<4,
    CPU_CAPACITY_SSE    =1<<5,
    CPU_CAPACITY_SSE2   =1<<6,
    CPU_CAPACITY_SSE3   =1<<7,
    CPU_CAPACITY_SSSE3  =1<<8,
    CPU_CAPACITY_ALTIVEC=1<<9,

    CPUCAP_ALL=0xffffffff
} CPUCapacity;

class cpu_capacity
{
public:
    static quint32      _cpu_cap;
    static quint32      _cpu_mask;
public:
    cpu_capacity();

    static void 	init( void);
    static quint8 	hasMMX (void) {CHECK_Z(MMX)}
    static quint8 	hasMMXEXT (void){CHECK_Z(MMXEXT)}
    static quint8 	has3DNOW (void) {CHECK_Z(3DNOW)}
    static quint8 	hasSSE (void) {CHECK_Z(SSE)}
    static quint8 	hasSSE2 (void){CHECK_Z(SSE2)}
    static quint8 	hasSSE3 (void){CHECK_Z(SSE3)}
    static quint8 	hasSSSE3 (void){CHECK_Z(SSSE3)}
    static quint8 	has3DNOWEXT(void){CHECK_Z(3DNOWEXT)}

    int cpu_processors_count(void);
};

#endif // CPU_SUPPORT_H
