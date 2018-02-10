#include "memcpy_cpu.h"

#ifdef CPU_X86
#define SSE_MMREG_SIZE 16
#define MMX_MMREG_SIZE 8

#define MMX1_MIN_LEN 0x800  /* 2K blocks */
#define MIN_LEN 0x40  /* 64-byte blocks */

//! 小于256字节则用此函数最快
#define small_memcpy(to, from, n)\
{\
register unsigned long int dummy;\
__asm__ __volatile__(\
  "rep; movsb"\
  :"=&D"(to), "=&S"(from), "=&c"(dummy)\
  :"0" (to), "1" (from),"2" (n)\
  : "memory");\
}

/* linux kernel __memcpy */
static __inline__ void * linux_kernel_memcpy_impl (
                   void * to,
                   const void * from,
                   size_t n)
{
int d0, d1, d2;

  if( n < 4 ) {
    small_memcpy(to,from,n);
  }
  else
    __asm__ __volatile__(
    "rep ; movsl\n\t"
    "testb $2,%b4\n\t"
    "je 1f\n\t"
    "movsw\n"
    "1:\ttestb $1,%b4\n\t"
    "je 2f\n\t"
    "movsb\n"
    "2:"
    : "=&c" (d0), "=&D" (d1), "=&S" (d2)
    :"0" (n/4), "q" (n),"1" ((intptr_t) to),"2" ((intptr_t) from)
    : "memory");

  return (to);
}
static void * sse_memcpy(void * to, void * from, size_t len)
{
  void *retval;
  quint8 *ufrom=(quint8 *)from;
  quint8 *uto=(quint8 *)to;
  retval = to;

  /* PREFETCH has effect even for MOVSB instruction ;) */
  __asm__ __volatile__ (
    "   prefetchnta (%0)\n"
    "   prefetchnta 32(%0)\n"
    "   prefetchnta 64(%0)\n"
    "   prefetchnta 96(%0)\n"
    "   prefetchnta 128(%0)\n"
    "   prefetchnta 160(%0)\n"
    "   prefetchnta 192(%0)\n"
    "   prefetchnta 224(%0)\n"
    "   prefetchnta 256(%0)\n"
    "   prefetchnta 288(%0)\n"
    : : "r" (ufrom) );

  if(len >= MIN_LEN)
  {
    register unsigned long int delta;
    /* Align destinition to MMREG_SIZE -boundary */
    delta = ((intptr_t)uto)&(SSE_MMREG_SIZE-1);
    if(delta)
    {
      delta=SSE_MMREG_SIZE-delta;
      len -= delta;
      small_memcpy(uto, ufrom, delta);
    }
    size_t i = len >> 6; /* len/64 */
    len&=63;
    if(((intptr_t)ufrom) & 15)
      /* if SRC is misaligned */
      for(; i>0; i--)
      {
        __asm__ __volatile__ (
        "prefetchnta 320(%0)\n"
       "prefetchnta 352(%0)\n"
        "movups (%0), %%xmm0\n"
        "movups 16(%0), %%xmm1\n"
        "movups 32(%0), %%xmm2\n"
        "movups 48(%0), %%xmm3\n"
        "movntps %%xmm0, (%1)\n"
        "movntps %%xmm1, 16(%1)\n"
        "movntps %%xmm2, 32(%1)\n"
        "movntps %%xmm3, 48(%1)\n"
        :: "r" (ufrom), "r" (uto) : "memory");
         ufrom+=64;
         uto+=64;
      }
    else
      /*
         Only if SRC is aligned on 16-byte boundary.
         It allows to use movaps instead of movups, which required data
         to be aligned or a general-protection exception (#GP) is generated.
      */
      for(; i>0; i--)
      {
        __asm__ __volatile__ (
        "prefetchnta 320(%0)\n"
       "prefetchnta 352(%0)\n"
        "movaps (%0), %%xmm0\n"
        "movaps 16(%0), %%xmm1\n"
        "movaps 32(%0), %%xmm2\n"
        "movaps 48(%0), %%xmm3\n"
        "movntps %%xmm0, (%1)\n"
        "movntps %%xmm1, 16(%1)\n"
        "movntps %%xmm2, 32(%1)\n"
        "movntps %%xmm3, 48(%1)\n"
        :: "r" (ufrom), "r" (uto) : "memory");
        ufrom+=64;
        uto+=64;
      }
    /* since movntq is weakly-ordered, a "sfence"
     * is needed to become ordered again. */
    __asm__ __volatile__ ("sfence":::"memory");
    /* enables to use FPU */
    __asm__ __volatile__ ("emms":::"memory");
  }
  /*
   *	Now do the tail of the block
   */
  if(len) linux_kernel_memcpy_impl(uto, ufrom, len);
  return retval;
}

static void * mmx_memcpy(void * to, const void * from, size_t len)
{
  void *retval;
  quint8 *ufrom=(quint8 *)from;
  quint8 *uto=(quint8 *)to;
  retval = uto;

  if(len >= MMX1_MIN_LEN)
  {
    register unsigned long int delta;
    /* Align destinition to MMREG_SIZE -boundary */
    delta = ((intptr_t)uto)&(MMX_MMREG_SIZE-1);
    if(delta)
    {
      delta=MMX_MMREG_SIZE-delta;
      len -= delta;
      small_memcpy(uto, ufrom, delta);
    }
    size_t i = len >> 6; /* len/64 */
    len&=63;
    for(; i>0; i--)
    {
      __asm__ __volatile__ (
      "movq (%0), %%mm0\n"
      "movq 8(%0), %%mm1\n"
      "movq 16(%0), %%mm2\n"
      "movq 24(%0), %%mm3\n"
      "movq 32(%0), %%mm4\n"
      "movq 40(%0), %%mm5\n"
      "movq 48(%0), %%mm6\n"
      "movq 56(%0), %%mm7\n"
      "movq %%mm0, (%1)\n"
      "movq %%mm1, 8(%1)\n"
      "movq %%mm2, 16(%1)\n"
      "movq %%mm3, 24(%1)\n"
      "movq %%mm4, 32(%1)\n"
      "movq %%mm5, 40(%1)\n"
      "movq %%mm6, 48(%1)\n"
      "movq %%mm7, 56(%1)\n"
      :: "r" (ufrom), "r" (uto) : "memory");
      ufrom+=64;
      uto+=64;
    }
    __asm__ __volatile__ ("emms":::"memory");
  }
  /*
   *	Now do the tail of the block
   */
  if(len) linux_kernel_memcpy_impl(uto, ufrom, len);
  return retval;
}

static void * mmx2_memcpy(void * to, const void * from, size_t len)
{
  void *retval;
  quint8 *ufrom=(quint8 *)from;
  quint8 *uto=(quint8 *)to;
  retval = to;

  /* PREFETCH has effect even for MOVSB instruction ;) */
  __asm__ __volatile__ (
    "   prefetchnta (%0)\n"
    "   prefetchnta 32(%0)\n"
    "   prefetchnta 64(%0)\n"
    "   prefetchnta 96(%0)\n"
    "   prefetchnta 128(%0)\n"
    "   prefetchnta 160(%0)\n"
    "   prefetchnta 192(%0)\n"
    "   prefetchnta 224(%0)\n"
    "   prefetchnta 256(%0)\n"
    "   prefetchnta 288(%0)\n"
    : : "r" (ufrom) );

  if(len >= MIN_LEN)
  {
    register unsigned long int delta;
    /* Align destinition to MMREG_SIZE -boundary */
    delta = ((intptr_t)uto)&(MMX_MMREG_SIZE-1);
    if(delta)
    {
      delta=MMX_MMREG_SIZE-delta;
      len -= delta;
      small_memcpy(uto, ufrom, delta);
    }
    size_t i = len >> 6; /* len/64 */
    len&=63;
    for(; i>0; i--)
    {
      __asm__ __volatile__ (
      "prefetchnta 320(%0)\n"
      "prefetchnta 352(%0)\n"
      "movq (%0), %%mm0\n"
      "movq 8(%0), %%mm1\n"
      "movq 16(%0), %%mm2\n"
      "movq 24(%0), %%mm3\n"
      "movq 32(%0), %%mm4\n"
      "movq 40(%0), %%mm5\n"
      "movq 48(%0), %%mm6\n"
      "movq 56(%0), %%mm7\n"
      "movntq %%mm0, (%1)\n"
      "movntq %%mm1, 8(%1)\n"
      "movntq %%mm2, 16(%1)\n"
      "movntq %%mm3, 24(%1)\n"
      "movntq %%mm4, 32(%1)\n"
      "movntq %%mm5, 40(%1)\n"
      "movntq %%mm6, 48(%1)\n"
      "movntq %%mm7, 56(%1)\n"
      :: "r" (ufrom), "r" (uto) : "memory");
      ufrom+=64;
      uto+=64;
    }
     /* since movntq is weakly-ordered, a "sfence"
     * is needed to become ordered again. */
    __asm__ __volatile__ ("sfence":::"memory");
    __asm__ __volatile__ ("emms":::"memory");
  }
  /*
   *	Now do the tail of the block
   */
  if(len) linux_kernel_memcpy_impl(uto, ufrom, len);
  return retval;
}
static void *linux_kernel_memcpy(void *to, const void *from, size_t len) {
  return linux_kernel_memcpy_impl(to,from,len);
}
#endif /* ARCH_X86 */

#if defined(CPU_X86)
static unsigned long long int rdtsc(void)
{
  unsigned long long int x=0;

    __asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
    return x;
 }
#else
static unsigned long long int rdtsc(void)
{
    return 0;
}

#endif

memcpy_cpu::memcpy_cpu()
{
    #undef memcpy
}

void memcpy_cpu::memcpy(void * to, const void * from, size_t len)
{
#if defined(CPU_X86)
        if(cpu_capacity::hasMMX())
           mmx_memcpy(to, from, len);
#endif
}
