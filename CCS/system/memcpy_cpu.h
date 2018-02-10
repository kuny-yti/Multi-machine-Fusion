#ifndef MEMCPY_CPU_H
#define MEMCPY_CPU_H
#include "cpu_capacity.h"

class memcpy_cpu
{
public:
    memcpy_cpu();

    void init_memcpy();
    static void memcpy(void *to, const void *from, size_t len);
};

#endif // MEMCPY_CPU_H
