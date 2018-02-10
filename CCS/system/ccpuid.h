
#ifndef __CCPUID_H_INCLUDED
#define __CCPUID_H_INCLUDED

#include <stddef.h>	// NULL等标准宏和类型.

// SIGILL信号处理.
//#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>

// 统一使用“uint32_t”	//#include <basetsd.h>	// INT32、UINT32等规范类型名.
#include "stdint.h"

typedef unsigned int uint32_t;
// intrinsics
#if (defined(__GNUC__))	// GCC
    #if (defined(__i386__) || defined(__x86_64__))
        #define CCPUID_X86	1	// 是x86平台.
        //#include <xmmintrin.h>	// MMX, SSE
        #ifndef __cpuid_count
            #if defined(__i386__) && defined(__PIC__)
            #if __GNUC__ >= 3
            #define __cpuid(level, a, b, c, d)			\
              __asm__ ("xchg{l}\t{%%}ebx, %1\n\t"			\
                   "cpuid\n\t"					\
                   "xchg{l}\t{%%}ebx, %1\n\t"			\
                   : "=a" (a), "=r" (b), "=c" (c), "=d" (d)	\
                   : "0" (level))

            #define __cpuid_count(level, count, a, b, c, d)		\
              __asm__ ("xchg{l}\t{%%}ebx, %1\n\t"			\
                   "cpuid\n\t"					\
                   "xchg{l}\t{%%}ebx, %1\n\t"			\
                   : "=a" (a), "=r" (b), "=c" (c), "=d" (d)	\
                   : "0" (level), "2" (count))
            #else
            /* Host GCCs older than 3.0 weren't supporting Intel asm syntax
               nor alternatives in i386 code.  */
            #define __cpuid(level, a, b, c, d)			\
              __asm__ ("xchgl\t%%ebx, %1\n\t"			\
                   "cpuid\n\t"					\
                   "xchgl\t%%ebx, %1\n\t"			\
                   : "=a" (a), "=r" (b), "=c" (c), "=d" (d)	\
                   : "0" (level))

            #define __cpuid_count(level, count, a, b, c, d)		\
              __asm__ ("xchgl\t%%ebx, %1\n\t"			\
                   "cpuid\n\t"					\
                   "xchgl\t%%ebx, %1\n\t"			\
                   : "=a" (a), "=r" (b), "=c" (c), "=d" (d)	\
                   : "0" (level), "2" (count))
            #endif

            #else
            #define __cpuid(level, a, b, c, d)			\
              __asm__ ("cpuid\n\t"					\
                   : "=a" (a), "=b" (b), "=c" (c), "=d" (d)	\
                   : "0" (level))

            #define __cpuid_count(level, count, a, b, c, d)		\
              __asm__ ("cpuid\n\t"					\
                   : "=a" (a), "=b" (b), "=c" (c), "=d" (d)	\
                   : "0" (level), "2" (count))
            #endif
        #endif	// #ifndef __cpuid_count
    #endif	// #if (defined(__i386__) || defined(__x86_64__) )

#elif defined(_MSC_VER)	// MSVC
    #if (defined(_M_IX86) || defined(_M_X64))
        #define CCPUID_X86	1	// 是x86平台.
        #if _MSC_VER >=1400	// VC2005
            #include <intrin.h>	// CPUID, MMX, SSE
        #elif _MSC_VER >=1200	// VC6
            #include <xmmintrin.h>	// MMX, SSE
        #endif	// #if _MSC_VER >=1400
    #endif

#else
    #error Only supports MSVC or GCC.
#endif	// #if defined(__GNUC__)
//! CPU ID 函数

// INLINE
#ifndef INLINE
    #if defined(_MSC_VER)	// MSVC
        #define INLINE	__inline
    #else	// C99
        #define INLINE	inline
    #endif
#endif


#if defined __cplusplus
extern "C" {
#endif

////////////////////////////////////////
// getcpuid: 获取CPUID信息的基础函数.
////////////////////////////////////////

INLINE void cpu_idex(uint32_t CPUInfo[4], uint32_t InfoType, uint32_t ECXValue)
{
    if (NULL == CPUInfo)	return;

#if defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))	// GCC
    __cpuid_count(InfoType, ECXValue, CPUInfo[0],CPUInfo[1],CPUInfo[2],CPUInfo[3]);
#elif defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64))	// MSVC
    #if defined(_M_X64) || _MSC_VER>=1600	// 64位下不支持内联汇编. 1600: VS2010, 据说VC2008 SP1之后才支持__cpuidex.
        __cpuidex((int*)CPUInfo, (int)InfoType, (int)ECXValue);
    #else
        _asm{
            // load. 读取参数到寄存器.
            mov edi, CPUInfo;	// 准备用edi寻址CPUInfo
            mov eax, InfoType;
            mov ecx, ECXValue;
            // CPUID
            cpuid;
            // save. 将寄存器保存到CPUInfo
            mov	[edi], eax;
            mov	[edi+4], ebx;
            mov	[edi+8], ecx;
            mov	[edi+12], edx;
        }
    #endif
#else
    // 不支持，返回0 .
    CPUInfo[0] = CPUInfo[1] = CPUInfo[2] = CPUInfo[3] = 0;
#endif	// #if defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))
}

INLINE void cpuid(uint32_t CPUInfo[4], uint32_t InfoType)
{
#if defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))	// GCC
    __cpuid(InfoType, CPUInfo[0],CPUInfo[1],CPUInfo[2],CPUInfo[3]);
#elif defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64))	// MSVC
    #if _MSC_VER>=1400	// VC2005才支持__cpuid
        __cpuid((int*)CPUInfo, (int)InfoType);
    #else
        cpu_idex(CPUInfo, InfoType, 0);
    #endif
#else
    cpu_idex(CPUInfo, InfoType, 0);
#endif	// #if defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))
}


// 存储单条CPUID信息的结构体.
typedef struct tagCPUIDINFO{
    uint32_t	fid;	// 功能号. 即CPUID指令的EAX参数.
    uint32_t	fidsub;	// 子功能号. 即CPUID指令的ECX参数.
    union{
        uint32_t	dw[4];	// 返回的信息.
        struct{
            uint32_t	_eax;
            uint32_t	_ebx;
            uint32_t	_ecx;
            uint32_t	_edx;
        };
    };
}CPUIDINFO, *LPCPUIDINFO;
typedef const CPUIDINFO* LPCCPUIDINFO;


typedef uint32_t CPUIDFIELD;

#define  CPUIDFIELD_MASK_POS	0x0000001F	// 位偏移. 0~31.
#define  CPUIDFIELD_MASK_LEN	0x000003E0	// 位长. 1~32
#define  CPUIDFIELD_MASK_REG	0x00000C00	// 寄存器. 0=EAX, 1=EBX, 2=ECX, 3=EDX.
#define  CPUIDFIELD_MASK_FIDSUB	0x000FF000	// 子功能号(低8位).
#define  CPUIDFIELD_MASK_FID	0xFFF00000	// 功能号(最高4位 和 低8位).

#define CPUIDFIELD_SHIFT_POS	0
#define CPUIDFIELD_SHIFT_LEN	5
#define CPUIDFIELD_SHIFT_REG	10
#define CPUIDFIELD_SHIFT_FIDSUB	12
#define CPUIDFIELD_SHIFT_FID	20

#define CPUIDFIELD_MAKE(fid,fidsub,reg,pos,len)	(((fid)&0xF0000000U) \
    | ((fid)<<CPUIDFIELD_SHIFT_FID & 0x0FF00000) \
    | ((fidsub)<<CPUIDFIELD_SHIFT_FIDSUB & CPUIDFIELD_MASK_FIDSUB) \
    | ((reg)<<CPUIDFIELD_SHIFT_REG & CPUIDFIELD_MASK_REG) \
    | ((pos)<<CPUIDFIELD_SHIFT_POS & CPUIDFIELD_MASK_POS) \
    | (((len)-1)<<CPUIDFIELD_SHIFT_LEN & CPUIDFIELD_MASK_LEN) \
    )
#define CPUIDFIELD_FID(cpuidfield)	( ((uint32_t)(cpuidfield)&0xF0000000U) | (((uint32_t)(cpuidfield) & 0x0FF00000)>>CPUIDFIELD_SHIFT_FID) )
#define CPUIDFIELD_FIDSUB(cpuidfield)	( ((cpuidfield) & CPUIDFIELD_MASK_FIDSUB)>>CPUIDFIELD_SHIFT_FIDSUB )
#define CPUIDFIELD_REG(cpuidfield)	( ((cpuidfield) & CPUIDFIELD_MASK_REG)>>CPUIDFIELD_SHIFT_REG )
#define CPUIDFIELD_POS(cpuidfield)	( ((cpuidfield) & CPUIDFIELD_MASK_POS)>>CPUIDFIELD_SHIFT_POS )
#define CPUIDFIELD_LEN(cpuidfield)	( (((cpuidfield) & CPUIDFIELD_MASK_LEN)>>CPUIDFIELD_SHIFT_LEN) + 1 )

// CPUID字段的描述.
typedef struct tagCPUIDFIELDDESC{
    CPUIDFIELD	cpuf;
    int	reserved;
    const char* szName;
    const char* szDesc;
}CPUIDFIELDDESC, *LPCPUIDFIELDDESC;
typedef const CPUIDFIELDDESC* LPCCPUIDFIELDDESC;

#define CPUF_LFuncStd	CPUIDFIELD_MAKE(0,0,0,0,32)
#define CPUF_Stepping	CPUIDFIELD_MAKE(1,0,0,0,4)
#define CPUF_BaseModel	CPUIDFIELD_MAKE(1,0,0,4,4)
#define CPUF_BaseFamily	CPUIDFIELD_MAKE(1,0,0,8,4)
#define CPUF_ProcessorType	CPUIDFIELD_MAKE(1,0,0,12,2)
#define CPUF_ExtModel	CPUIDFIELD_MAKE(1,0,0,16,4)
#define CPUF_ExtFamily	CPUIDFIELD_MAKE(1,0,0,20,8)
#define CPUF_BrandId8	CPUIDFIELD_MAKE(1,0,1,0,8)
#define CPUF_CLFlush	CPUIDFIELD_MAKE(1,0,1,8,8)
#define CPUF_MaxApicId	CPUIDFIELD_MAKE(1,0,1,16,8)
#define CPUF_ApicId	CPUIDFIELD_MAKE(1,0,1,24,8)
#define CPUF_SSE3	CPUIDFIELD_MAKE(1,0,2,0,1)
#define CPUF_PCLMULQDQ	CPUIDFIELD_MAKE(1,0,2,1,1)
#define CPUF_DTES64	CPUIDFIELD_MAKE(1,0,2,2,1)
#define CPUF_MONITOR	CPUIDFIELD_MAKE(1,0,2,3,1)
#define CPUF_DS_CPL	CPUIDFIELD_MAKE(1,0,2,4,1)
#define CPUF_VMX	CPUIDFIELD_MAKE(1,0,2,5,1)
#define CPUF_SMX	CPUIDFIELD_MAKE(1,0,2,6,1)
#define CPUF_EIST	CPUIDFIELD_MAKE(1,0,2,7,1)
#define CPUF_TM2	CPUIDFIELD_MAKE(1,0,2,8,1)
#define CPUF_SSSE3	CPUIDFIELD_MAKE(1,0,2,9,1)
#define CPUF_CNXT_ID	CPUIDFIELD_MAKE(1,0,2,10,1)
#define CPUF_FMA	CPUIDFIELD_MAKE(1,0,2,12,1)
#define CPUF_CX16	CPUIDFIELD_MAKE(1,0,2,13,1)
#define CPUF_xTPR	CPUIDFIELD_MAKE(1,0,2,14,1)
#define CPUF_PDCM	CPUIDFIELD_MAKE(1,0,2,15,1)
#define CPUF_PCID	CPUIDFIELD_MAKE(1,0,2,17,1)
#define CPUF_DCA	CPUIDFIELD_MAKE(1,0,2,18,1)
#define CPUF_SSE41	CPUIDFIELD_MAKE(1,0,2,19,1)
#define CPUF_SSE42	CPUIDFIELD_MAKE(1,0,2,20,1)
#define CPUF_x2APIC	CPUIDFIELD_MAKE(1,0,2,21,1)
#define CPUF_MOVBE	CPUIDFIELD_MAKE(1,0,2,22,1)
#define CPUF_POPCNT	CPUIDFIELD_MAKE(1,0,2,23,1)
#define CPUF_TSC_DEADLINE	CPUIDFIELD_MAKE(1,0,2,24,1)
#define CPUF_AES	CPUIDFIELD_MAKE(1,0,2,25,1)
#define CPUF_XSAVE	CPUIDFIELD_MAKE(1,0,2,26,1)
#define CPUF_OSXSAVE	CPUIDFIELD_MAKE(1,0,2,27,1)
#define CPUF_AVX	CPUIDFIELD_MAKE(1,0,2,28,1)
#define CPUF_F16C	CPUIDFIELD_MAKE(1,0,2,29,1)
#define CPUF_RDRAND	CPUIDFIELD_MAKE(1,0,2,30,1)
#define CPUF_FPU	CPUIDFIELD_MAKE(1,0,3,0,1)
#define CPUF_VME	CPUIDFIELD_MAKE(1,0,3,1,1)
#define CPUF_DE	CPUIDFIELD_MAKE(1,0,3,2,1)
#define CPUF_PSE	CPUIDFIELD_MAKE(1,0,3,3,1)
#define CPUF_TSC	CPUIDFIELD_MAKE(1,0,3,4,1)
#define CPUF_MSR	CPUIDFIELD_MAKE(1,0,3,5,1)
#define CPUF_PAE	CPUIDFIELD_MAKE(1,0,3,6,1)
#define CPUF_MCE	CPUIDFIELD_MAKE(1,0,3,7,1)
#define CPUF_CX8	CPUIDFIELD_MAKE(1,0,3,8,1)
#define CPUF_APIC	CPUIDFIELD_MAKE(1,0,3,9,1)
#define CPUF_SEP	CPUIDFIELD_MAKE(1,0,3,11,1)
#define CPUF_MTRR	CPUIDFIELD_MAKE(1,0,3,12,1)
#define CPUF_PGE	CPUIDFIELD_MAKE(1,0,3,13,1)
#define CPUF_MCA	CPUIDFIELD_MAKE(1,0,3,14,1)
#define CPUF_CMOV	CPUIDFIELD_MAKE(1,0,3,15,1)
#define CPUF_PAT	CPUIDFIELD_MAKE(1,0,3,16,1)
#define CPUF_PSE36	CPUIDFIELD_MAKE(1,0,3,17,1)
#define CPUF_PSN	CPUIDFIELD_MAKE(1,0,3,18,1)
#define CPUF_CLFSH	CPUIDFIELD_MAKE(1,0,3,19,1)
#define CPUF_DS	CPUIDFIELD_MAKE(1,0,3,21,1)
#define CPUF_ACPI	CPUIDFIELD_MAKE(1,0,3,22,1)
#define CPUF_MMX	CPUIDFIELD_MAKE(1,0,3,23,1)
#define CPUF_FXSR	CPUIDFIELD_MAKE(1,0,3,24,1)
#define CPUF_SSE	CPUIDFIELD_MAKE(1,0,3,25,1)
#define CPUF_SSE2	CPUIDFIELD_MAKE(1,0,3,26,1)
#define CPUF_SS	CPUIDFIELD_MAKE(1,0,3,27,1)
#define CPUF_HTT	CPUIDFIELD_MAKE(1,0,3,28,1)
#define CPUF_TM	CPUIDFIELD_MAKE(1,0,3,29,1)
#define CPUF_PBE	CPUIDFIELD_MAKE(1,0,3,31,1)
#define CPUF_Cache_Type	CPUIDFIELD_MAKE(4,0,0,0,5)
#define CPUF_Cache_Level	CPUIDFIELD_MAKE(4,0,0,5,3)
#define CPUF_CACHE_SI	CPUIDFIELD_MAKE(4,0,0,8,1)
#define CPUF_CACHE_FA	CPUIDFIELD_MAKE(4,0,0,9,1)
#define CPUF_MaxApicIdShare	CPUIDFIELD_MAKE(4,0,0,14,12)
#define CPUF_MaxApicIdCore	CPUIDFIELD_MAKE(4,0,0,26,6)
#define CPUF_Cache_LineSize	CPUIDFIELD_MAKE(4,0,1,0,12)
#define CPUF_Cache_Partitions	CPUIDFIELD_MAKE(4,0,1,12,10)
#define CPUF_Cache_Ways	CPUIDFIELD_MAKE(4,0,1,22,10)
#define CPUF_Cache_Sets	CPUIDFIELD_MAKE(4,0,2,0,32)
#define CPUF_CACHE_INVD	CPUIDFIELD_MAKE(4,0,3,0,1)
#define CPUF_CACHE_INCLUSIVENESS	CPUIDFIELD_MAKE(4,0,3,1,1)
#define CPUF_CACHE_COMPLEXINDEX	CPUIDFIELD_MAKE(4,0,3,2,1)
#define CPUF_MonLineSizeMin	CPUIDFIELD_MAKE(5,0,0,0,16)
#define CPUF_MonLineSizeMax	CPUIDFIELD_MAKE(5,0,1,0,16)
#define CPUF_EMX	CPUIDFIELD_MAKE(5,0,2,0,1)
#define CPUF_IBE	CPUIDFIELD_MAKE(5,0,2,1,1)
#define CPUF_MWAIT_Number_C0	CPUIDFIELD_MAKE(5,0,3,0,4)
#define CPUF_MWAIT_Number_C1	CPUIDFIELD_MAKE(5,0,3,4,4)
#define CPUF_MWAIT_Number_C2	CPUIDFIELD_MAKE(5,0,3,8,4)
#define CPUF_MWAIT_Number_C3	CPUIDFIELD_MAKE(5,0,3,12,4)
#define CPUF_MWAIT_Number_C4	CPUIDFIELD_MAKE(5,0,3,16,4)
#define CPUF_DTS	CPUIDFIELD_MAKE(6,0,0,0,1)
#define CPUF_TURBO_BOOST	CPUIDFIELD_MAKE(6,0,0,1,1)
#define CPUF_ARAT	CPUIDFIELD_MAKE(6,0,0,2,1)
#define CPUF_PLN	CPUIDFIELD_MAKE(6,0,0,4,1)
#define CPUF_ECMD	CPUIDFIELD_MAKE(6,0,0,5,1)
#define CPUF_PTM	CPUIDFIELD_MAKE(6,0,0,6,1)
#define CPUF_DTS_ITs	CPUIDFIELD_MAKE(6,0,1,0,4)
#define CPUF_PERF	CPUIDFIELD_MAKE(6,0,2,0,1)
#define CPUF_ACNT2	CPUIDFIELD_MAKE(6,0,2,1,1)
#define CPUF_ENERGY_PERF_BIAS	CPUIDFIELD_MAKE(6,0,2,3,1)
#define CPUF_Max07Subleaf	CPUIDFIELD_MAKE(7,0,0,0,32)
#define CPUF_FSGSBASE	CPUIDFIELD_MAKE(7,0,1,0,1)
#define CPUF_TSC_ADJUST	CPUIDFIELD_MAKE(7,0,1,1,1)
#define CPUF_BMI1	CPUIDFIELD_MAKE(7,0,1,3,1)
#define CPUF_HLE	CPUIDFIELD_MAKE(7,0,1,4,1)
#define CPUF_AVX2	CPUIDFIELD_MAKE(7,0,1,5,1)
#define CPUF_SMEP	CPUIDFIELD_MAKE(7,0,1,7,1)
#define CPUF_BMI2	CPUIDFIELD_MAKE(7,0,1,8,1)
#define CPUF_ERMS	CPUIDFIELD_MAKE(7,0,1,9,1)
#define CPUF_INVPCID	CPUIDFIELD_MAKE(7,0,1,10,1)
#define CPUF_RTM	CPUIDFIELD_MAKE(7,0,1,11,1)
#define CPUF_RDSEED	CPUIDFIELD_MAKE(7,0,1,18,1)
#define CPUF_ADX	CPUIDFIELD_MAKE(7,0,1,19,1)
#define CPUF_SMAP	CPUIDFIELD_MAKE(7,0,1,20,1)
#define CPUF_PLATFORM_DCA_CAP	CPUIDFIELD_MAKE(9,0,0,0,32)
#define CPUF_APM_Version	CPUIDFIELD_MAKE(0xA,0,0,0,8)
#define CPUF_APM_Counters	CPUIDFIELD_MAKE(0xA,0,0,8,8)
#define CPUF_APM_Bits	CPUIDFIELD_MAKE(0xA,0,0,16,8)
#define CPUF_APM_Length	CPUIDFIELD_MAKE(0xA,0,0,24,8)
#define CPUF_APM_CC	CPUIDFIELD_MAKE(0xA,0,1,0,1)
#define CPUF_APM_IR	CPUIDFIELD_MAKE(0xA,0,1,1,1)
#define CPUF_APM_RC	CPUIDFIELD_MAKE(0xA,0,1,2,1)
#define CPUF_APM_LLCR	CPUIDFIELD_MAKE(0xA,0,1,3,1)
#define CPUF_APM_LLCM	CPUIDFIELD_MAKE(0xA,0,1,4,1)
#define CPUF_APM_BIR	CPUIDFIELD_MAKE(0xA,0,1,5,1)
#define CPUF_APM_BMR	CPUIDFIELD_MAKE(0xA,0,1,6,1)
#define CPUF_APM_FC_Number	CPUIDFIELD_MAKE(0xA,0,3,0,5)
#define CPUF_APM_FC_Bits	CPUIDFIELD_MAKE(0xA,0,3,5,8)
#define CPUF_Topology_Bits	CPUIDFIELD_MAKE(0xB,0,0,0,5)
#define CPUF_Topology_Number	CPUIDFIELD_MAKE(0xB,0,1,0,16)
#define CPUF_Topology_Level	CPUIDFIELD_MAKE(0xB,0,2,0,8)
#define CPUF_Topology_Type	CPUIDFIELD_MAKE(0xB,0,2,8,8)
#define CPUF_X2APICID	CPUIDFIELD_MAKE(0xB,0,3,0,32)
#define CPUF_XFeatureSupportedMaskLo	CPUIDFIELD_MAKE(0xD,0,0,0,32)
#define CPUF_XFeatureEnabledSizeMax	CPUIDFIELD_MAKE(0xD,0,1,0,32)
#define CPUF_XFeatureSupportedSizeMax	CPUIDFIELD_MAKE(0xD,0,2,0,32)
#define CPUF_XFeatureSupportedMaskHi	CPUIDFIELD_MAKE(0xD,0,3,0,32)
#define CPUF_XSAVEOPT	CPUIDFIELD_MAKE(0xD,1,0,0,1)
#define CPUF_YmmSaveStateSize	CPUIDFIELD_MAKE(0xD,2,0,0,32)
#define CPUF_YmmSaveStateOffset	CPUIDFIELD_MAKE(0xD,2,1,0,32)
#define CPUF_LwpSaveStateSize	CPUIDFIELD_MAKE(0xD,62,0,0,32)
#define CPUF_LwpSaveStateOffset	CPUIDFIELD_MAKE(0xD,62,1,0,32)
#define CPUF_LFuncExt	CPUIDFIELD_MAKE(0x80000000U,0,0,0,32)
#define CPUF_BrandId16	CPUIDFIELD_MAKE(0x80000001U,0,1,0,16)
#define CPUF_PkgType	CPUIDFIELD_MAKE(0x80000001U,0,1,28,4)
#define CPUF_LahfSahf	CPUIDFIELD_MAKE(0x80000001U,0,2,0,1)
#define CPUF_CmpLegacy	CPUIDFIELD_MAKE(0x80000001U,0,2,1,1)
#define CPUF_SVM	CPUIDFIELD_MAKE(0x80000001U,0,2,2,1)
#define CPUF_ExtApicSpace	CPUIDFIELD_MAKE(0x80000001U,0,2,3,1)
#define CPUF_AltMovCr8	CPUIDFIELD_MAKE(0x80000001U,0,2,4,1)
#define CPUF_ABM	CPUIDFIELD_MAKE(0x80000001U,0,2,5,1)
#define CPUF_SSE4A	CPUIDFIELD_MAKE(0x80000001U,0,2,6,1)
#define CPUF_MisAlignSse	CPUIDFIELD_MAKE(0x80000001U,0,2,7,1)
#define CPUF_3DNowPrefetch	CPUIDFIELD_MAKE(0x80000001U,0,2,8,1)
#define CPUF_OSVW	CPUIDFIELD_MAKE(0x80000001U,0,2,9,1)
#define CPUF_IBS	CPUIDFIELD_MAKE(0x80000001U,0,2,10,1)
#define CPUF_XOP	CPUIDFIELD_MAKE(0x80000001U,0,2,11,1)
#define CPUF_SKINIT	CPUIDFIELD_MAKE(0x80000001U,0,2,12,1)
#define CPUF_WDT	CPUIDFIELD_MAKE(0x80000001U,0,2,13,1)
#define CPUF_LWP	CPUIDFIELD_MAKE(0x80000001U,0,2,15,1)
#define CPUF_FMA4	CPUIDFIELD_MAKE(0x80000001U,0,2,16,1)
#define CPUF_BIT_NODEID	CPUIDFIELD_MAKE(0x80000001U,0,2,19,1)
#define CPUF_TBM	CPUIDFIELD_MAKE(0x80000001U,0,2,21,1)
#define CPUF_TopologyExtensions	CPUIDFIELD_MAKE(0x80000001U,0,2,22,1)
#define CPUF_SYSCALL	CPUIDFIELD_MAKE(0x80000001U,0,3,11,1)
#define CPUF_XD	CPUIDFIELD_MAKE(0x80000001U,0,3,20,1)
#define CPUF_MmxExt	CPUIDFIELD_MAKE(0x80000001U,0,3,22,1)
#define CPUF_FFXSR	CPUIDFIELD_MAKE(0x80000001U,0,3,25,1)
#define CPUF_Page1GB	CPUIDFIELD_MAKE(0x80000001U,0,3,26,1)
#define CPUF_RDTSCP	CPUIDFIELD_MAKE(0x80000001U,0,3,27,1)
#define CPUF_LM	CPUIDFIELD_MAKE(0x80000001U,0,3,29,1)
#define CPUF_3DNowExt	CPUIDFIELD_MAKE(0x80000001U,0,3,30,1)
#define CPUF_3DNow	CPUIDFIELD_MAKE(0x80000001U,0,3,31,1)
#define CPUF_L1ITlb2and4MSize	CPUIDFIELD_MAKE(0x80000005U,0,0,0,8)
#define CPUF_L1ITlb2and4MAssoc	CPUIDFIELD_MAKE(0x80000005U,0,0,8,8)
#define CPUF_L1DTlb2and4MSize	CPUIDFIELD_MAKE(0x80000005U,0,0,16,8)
#define CPUF_L1DTlb2and4MAssoc	CPUIDFIELD_MAKE(0x80000005U,0,0,24,8)
#define CPUF_L1ITlb4KSize	CPUIDFIELD_MAKE(0x80000005U,0,1,0,8)
#define CPUF_L1ITlb4KAssoc	CPUIDFIELD_MAKE(0x80000005U,0,1,8,8)
#define CPUF_L1DTlb4KSize	CPUIDFIELD_MAKE(0x80000005U,0,1,16,8)
#define CPUF_L1DTlb4KAssoc	CPUIDFIELD_MAKE(0x80000005U,0,1,24,8)
#define CPUF_L1DcLineSize	CPUIDFIELD_MAKE(0x80000005U,0,2,0,8)
#define CPUF_L1DcLinesPerTag	CPUIDFIELD_MAKE(0x80000005U,0,2,8,8)
#define CPUF_L1DcAssoc	CPUIDFIELD_MAKE(0x80000005U,0,2,16,8)
#define CPUF_L1DcSize	CPUIDFIELD_MAKE(0x80000005U,0,2,24,8)
#define CPUF_L1IcLineSize	CPUIDFIELD_MAKE(0x80000005U,0,3,0,8)
#define CPUF_L1IcLinesPerTag	CPUIDFIELD_MAKE(0x80000005U,0,3,8,8)
#define CPUF_L1IcAssoc	CPUIDFIELD_MAKE(0x80000005U,0,3,16,8)
#define CPUF_L1IcSize	CPUIDFIELD_MAKE(0x80000005U,0,3,24,8)
#define CPUF_L2ITlb2and4MSize	CPUIDFIELD_MAKE(0x80000006U,0,0,0,12)
#define CPUF_L2ITlb2and4MAssoc	CPUIDFIELD_MAKE(0x80000006U,0,0,12,4)
#define CPUF_L2DTlb2and4MSize	CPUIDFIELD_MAKE(0x80000006U,0,0,16,12)
#define CPUF_L2DTlb2and4MAssoc	CPUIDFIELD_MAKE(0x80000006U,0,0,28,4)
#define CPUF_L2ITlb4KSize	CPUIDFIELD_MAKE(0x80000006U,0,1,0,12)
#define CPUF_L2ITlb4KAssoc	CPUIDFIELD_MAKE(0x80000006U,0,1,12,4)
#define CPUF_L2DTlb4KSize	CPUIDFIELD_MAKE(0x80000006U,0,1,16,12)
#define CPUF_L2DTlb4KAssoc	CPUIDFIELD_MAKE(0x80000006U,0,1,28,4)
#define CPUF_L2LineSize	CPUIDFIELD_MAKE(0x80000006U,0,2,0,8)
#define CPUF_L2LinesPerTag	CPUIDFIELD_MAKE(0x80000006U,0,2,8,4)
#define CPUF_L2Assoc	CPUIDFIELD_MAKE(0x80000006U,0,2,12,4)
#define CPUF_L2Size	CPUIDFIELD_MAKE(0x80000006U,0,2,16,16)
#define CPUF_L3LineSize	CPUIDFIELD_MAKE(0x80000006U,0,3,0,8)
#define CPUF_L3LinesPerTag	CPUIDFIELD_MAKE(0x80000006U,0,3,8,4)
#define CPUF_L3Assoc	CPUIDFIELD_MAKE(0x80000006U,0,3,12,4)
#define CPUF_L3Size	CPUIDFIELD_MAKE(0x80000006U,0,3,18,14)
#define CPUF_TS	CPUIDFIELD_MAKE(0x80000007U,0,3,0,1)
#define CPUF_FID	CPUIDFIELD_MAKE(0x80000007U,0,3,1,1)
#define CPUF_VID	CPUIDFIELD_MAKE(0x80000007U,0,3,2,1)
#define CPUF_TTP	CPUIDFIELD_MAKE(0x80000007U,0,3,3,1)
#define CPUF_HTC	CPUIDFIELD_MAKE(0x80000007U,0,3,4,1)
#define CPUF_100MHzSteps	CPUIDFIELD_MAKE(0x80000007U,0,3,6,1)
#define CPUF_HwPstate	CPUIDFIELD_MAKE(0x80000007U,0,3,7,1)
#define CPUF_TscInvariant	CPUIDFIELD_MAKE(0x80000007U,0,3,8,1)
#define CPUF_CPB	CPUIDFIELD_MAKE(0x80000007U,0,3,9,1)
#define CPUF_EffFreqRO	CPUIDFIELD_MAKE(0x80000007U,0,3,10,1)
#define CPUF_PhysAddrSize	CPUIDFIELD_MAKE(0x80000008U,0,0,0,8)
#define CPUF_LinAddrSize	CPUIDFIELD_MAKE(0x80000008U,0,0,8,8)
#define CPUF_GuestPhysAddrSize	CPUIDFIELD_MAKE(0x80000008U,0,0,16,8)
#define CPUF_NC	CPUIDFIELD_MAKE(0x80000008U,0,2,0,8)
#define CPUF_ApicIdCoreIdSize	CPUIDFIELD_MAKE(0x80000008U,0,2,12,4)
#define CPUF_SvmRev	CPUIDFIELD_MAKE(0x8000000AU,0,0,0,8)
#define CPUF_NASID	CPUIDFIELD_MAKE(0x8000000AU,0,1,0,32)
#define CPUF_NP	CPUIDFIELD_MAKE(0x8000000AU,0,3,0,1)
#define CPUF_LbrVirt	CPUIDFIELD_MAKE(0x8000000AU,0,3,1,1)
#define CPUF_SVML	CPUIDFIELD_MAKE(0x8000000AU,0,3,2,1)
#define CPUF_NRIPS	CPUIDFIELD_MAKE(0x8000000AU,0,3,3,1)
#define CPUF_TscRateMsr	CPUIDFIELD_MAKE(0x8000000AU,0,3,4,1)
#define CPUF_VmcbClean	CPUIDFIELD_MAKE(0x8000000AU,0,3,5,1)
#define CPUF_FlushByAsid	CPUIDFIELD_MAKE(0x8000000AU,0,3,6,1)
#define CPUF_DecodeAssists	CPUIDFIELD_MAKE(0x8000000AU,0,3,7,1)
#define CPUF_PauseFilter	CPUIDFIELD_MAKE(0x8000000AU,0,3,10,1)
#define CPUF_PauseFilterThreshold	CPUIDFIELD_MAKE(0x8000000AU,0,3,12,1)
#define CPUF_L1ITlb1GSize	CPUIDFIELD_MAKE(0x80000019U,0,0,0,12)
#define CPUF_L1ITlb1GAssoc	CPUIDFIELD_MAKE(0x80000019U,0,0,12,4)
#define CPUF_L1DTlb1GSize	CPUIDFIELD_MAKE(0x80000019U,0,0,16,12)
#define CPUF_L1DTlb1GAssoc	CPUIDFIELD_MAKE(0x80000019U,0,0,28,4)
#define CPUF_L2ITlb1GSize	CPUIDFIELD_MAKE(0x80000019U,0,1,0,12)
#define CPUF_L2ITlb1GAssoc	CPUIDFIELD_MAKE(0x80000019U,0,1,12,4)
#define CPUF_L2DTlb1GSize	CPUIDFIELD_MAKE(0x80000019U,0,1,16,12)
#define CPUF_L2DTlb1GAssoc	CPUIDFIELD_MAKE(0x80000019U,0,1,28,4)
#define CPUF_FP128	CPUIDFIELD_MAKE(0x8000001AU,0,0,0,1)
#define CPUF_MOVU	CPUIDFIELD_MAKE(0x8000001AU,0,0,1,1)
#define CPUF_IBSFFV	CPUIDFIELD_MAKE(0x8000001BU,0,0,0,1)
#define CPUF_FetchSam	CPUIDFIELD_MAKE(0x8000001BU,0,0,1,1)
#define CPUF_OpSam	CPUIDFIELD_MAKE(0x8000001BU,0,0,2,1)
#define CPUF_RdWrOpCnt	CPUIDFIELD_MAKE(0x8000001BU,0,0,3,1)
#define CPUF_OpCnt	CPUIDFIELD_MAKE(0x8000001BU,0,0,4,1)
#define CPUF_BrnTrgt	CPUIDFIELD_MAKE(0x8000001BU,0,0,5,1)
#define CPUF_OpCntExt	CPUIDFIELD_MAKE(0x8000001BU,0,0,6,1)
#define CPUF_RipInvalidChk	CPUIDFIELD_MAKE(0x8000001BU,0,0,7,1)
#define CPUF_LwpAvail	CPUIDFIELD_MAKE(0x8000001CU,0,0,0,1)
#define CPUF_LwpVAL	CPUIDFIELD_MAKE(0x8000001CU,0,0,1,1)
#define CPUF_LwpIRE	CPUIDFIELD_MAKE(0x8000001CU,0,0,2,1)
#define CPUF_LwpBRE	CPUIDFIELD_MAKE(0x8000001CU,0,0,3,1)
#define CPUF_LwpDME	CPUIDFIELD_MAKE(0x8000001CU,0,0,4,1)
#define CPUF_LwpCNH	CPUIDFIELD_MAKE(0x8000001CU,0,0,5,1)
#define CPUF_LwpRNH	CPUIDFIELD_MAKE(0x8000001CU,0,0,6,1)
#define CPUF_LwpInt	CPUIDFIELD_MAKE(0x8000001CU,0,0,31,1)
#define CPUF_LwpCbSize	CPUIDFIELD_MAKE(0x8000001CU,0,1,0,8)
#define CPUF_LwpEventSize	CPUIDFIELD_MAKE(0x8000001CU,0,1,8,8)
#define CPUF_LwpMaxEvents	CPUIDFIELD_MAKE(0x8000001CU,0,1,16,8)
#define CPUF_LwpEventOffset	CPUIDFIELD_MAKE(0x8000001CU,0,1,24,8)
#define CPUF_LwpLatencyMax	CPUIDFIELD_MAKE(0x8000001CU,0,2,0,5)
#define CPUF_LwpDataAddress	CPUIDFIELD_MAKE(0x8000001CU,0,2,5,1)
#define CPUF_LwpLatencyRnd	CPUIDFIELD_MAKE(0x8000001CU,0,2,6,3)
#define CPUF_LwpVersion	CPUIDFIELD_MAKE(0x8000001CU,0,2,9,7)
#define CPUF_LwpMinBufferSize	CPUIDFIELD_MAKE(0x8000001CU,0,2,16,8)
#define CPUF_LwpBranchPrediction	CPUIDFIELD_MAKE(0x8000001CU,0,2,28,1)
#define CPUF_LwpIpFiltering	CPUIDFIELD_MAKE(0x8000001CU,0,2,29,1)
#define CPUF_LwpCacheLevels	CPUIDFIELD_MAKE(0x8000001CU,0,2,30,1)
#define CPUF_LwpCacheLatency	CPUIDFIELD_MAKE(0x8000001CU,0,2,31,1)
#define CPUF_D_LwpAvail	CPUIDFIELD_MAKE(0x8000001CU,0,3,0,1)
#define CPUF_D_LwpVAL	CPUIDFIELD_MAKE(0x8000001CU,0,3,1,1)
#define CPUF_D_LwpIRE	CPUIDFIELD_MAKE(0x8000001CU,0,3,2,1)
#define CPUF_D_LwpBRE	CPUIDFIELD_MAKE(0x8000001CU,0,3,3,1)
#define CPUF_D_LwpDME	CPUIDFIELD_MAKE(0x8000001CU,0,3,4,1)
#define CPUF_D_LwpCNH	CPUIDFIELD_MAKE(0x8000001CU,0,3,5,1)
#define CPUF_D_LwpRNH	CPUIDFIELD_MAKE(0x8000001CU,0,3,6,1)
#define CPUF_D_LwpInt	CPUIDFIELD_MAKE(0x8000001CU,0,3,31,1)
#define CPUF_CacheType	CPUIDFIELD_MAKE(0x8000001DU,0,0,0,5)
#define CPUF_CacheLevel	CPUIDFIELD_MAKE(0x8000001DU,0,0,5,3)
#define CPUF_SelfInitialization	CPUIDFIELD_MAKE(0x8000001DU,0,0,8,1)
#define CPUF_FullyAssociative	CPUIDFIELD_MAKE(0x8000001DU,0,0,9,1)
#define CPUF_NumSharingCache	CPUIDFIELD_MAKE(0x8000001DU,0,0,14,12)
#define CPUF_CacheLineSize	CPUIDFIELD_MAKE(0x8000001DU,0,1,0,12)
#define CPUF_CachePhysPartitions	CPUIDFIELD_MAKE(0x8000001DU,0,1,12,10)
#define CPUF_CacheNumWays	CPUIDFIELD_MAKE(0x8000001DU,0,1,22,10)
#define CPUF_CacheNumSets	CPUIDFIELD_MAKE(0x8000001DU,0,2,0,32)
#define CPUF_WBINVD	CPUIDFIELD_MAKE(0x8000001DU,0,3,0,1)
#define CPUF_CacheInclusive	CPUIDFIELD_MAKE(0x8000001DU,0,3,1,1)
#define CPUF_ExtendedApicId	CPUIDFIELD_MAKE(0x8000001EU,0,0,0,32)
#define CPUF_ComputeUnitId	CPUIDFIELD_MAKE(0x8000001EU,0,1,0,8)
#define CPUF_CoresPerComputeUnit	CPUIDFIELD_MAKE(0x8000001EU,0,1,8,2)
#define CPUF_NodeId	CPUIDFIELD_MAKE(0x8000001EU,0,2,0,8)
#define CPUF_NodesPerProcessor	CPUIDFIELD_MAKE(0x8000001EU,0,2,8,3)


// 取得位域.
#ifndef __GETBITS32
#define __GETBITS32(src,pos,len)	( ((src)>>(pos)) & (((uint32_t)-1)>>(32-len)) )
#endif

// 根据CPUIDFIELD从缓冲区中获取字段.
INLINE uint32_t	cpuid_field_buff(const uint32_t dwBuf[4], CPUIDFIELD cpuf)
{
    return __GETBITS32(dwBuf[CPUIDFIELD_REG(cpuf)], CPUIDFIELD_POS(cpuf), CPUIDFIELD_LEN(cpuf));
}

// 根据CPUIDFIELD获取CPUID字段.
INLINE uint32_t	cpuid_field(CPUIDFIELD cpuf)
{
    uint32_t dwBuf[4];
    cpu_idex(dwBuf, CPUIDFIELD_FID(cpuf), CPUIDFIELD_FIDSUB(cpuf));
    return cpuid_field_buff(dwBuf, cpuf);
}

// SSE系列指令集的支持级别. simd_sse_level 函数的返回值.
#define SIMD_SSE_NONE	0	// 不支持.
#define SIMD_SSE_1	1	// SSE
#define SIMD_SSE_2	2	// SSE2
#define SIMD_SSE_3	3	// SSE3
#define SIMD_SSE_3S	4	// SSSE3
#define SIMD_SSE_41	5	// SSE4.1
#define SIMD_SSE_42	6	// SSE4.2


// AVX系列指令集的支持级别. simd_avx_level 函数的返回值。
#define SIMD_AVX_NONE	0	// 不支持
#define SIMD_AVX_1	1	// AVX
#define SIMD_AVX_2	2	// AVX2


// 取得CPU厂商（Vendor）.
//
// result: 成功时返回字符串的长度（一般为12）。失败时返回0.
// pvendor: 接收厂商信息的字符串缓冲区。至少为13字节.
INLINE int cpu_vendor(char* pvendor)
{
    uint32_t dwBuf[4];
    if (NULL == pvendor)	return 0;
    // Function 0: Vendor-ID and Largest Standard Function
    cpuid(dwBuf, 0);
    // save. 保存到pvendor
    *(uint32_t *)&pvendor[0] = dwBuf[1];	// ebx: 前四个字符.
    *(uint32_t *)&pvendor[4] = dwBuf[3];	// edx: 中间四个字符.
    *(uint32_t *)&pvendor[8] = dwBuf[2];	// ecx: 最后四个字符.
    pvendor[12] = '\0';
    return 12;
}

// 取得CPU商标（Brand）.
//
// result: 成功时返回字符串的长度（一般为48）。失败时返回0.
// pbrand: 接收商标信息的字符串缓冲区。至少为49字节.
INLINE int cpu_brand(char* pbrand)
{
    uint32_t dwBuf[4];
    if (NULL == pbrand)	return 0;
    // Function 0x80000000: Largest Extended Function Number
    cpuid(dwBuf, 0x80000000U);
    if (dwBuf[0] < 0x80000004U)	return 0;
    // Function 80000002h,80000003h,80000004h: Processor Brand String
    cpuid((uint32_t *)&pbrand[0], 0x80000002U);	// 前16个字符.
    cpuid((uint32_t *)&pbrand[16], 0x80000003U);	// 中间16个字符.
    cpuid((uint32_t *)&pbrand[32], 0x80000004U);	// 最后16个字符.
    pbrand[48] = '\0';
    return 48;
}

// 收到SIGILL信号时的跳回地址.
static jmp_buf *volatile simd_pjmpback_sigill = NULL;	// *volatile表示该指针本身是易失。如果不加volatile关键字，在Release模式编译时，编译器有可能会优化掉对指针的访问，使得simd_catch_sigill函数中总是取得NULL.

// 处理SIGILL信号.
static void simd_catch_sigill(int )
{
    jmp_buf * pjmp = simd_pjmpback_sigill;
    // 能够跳回.
    if (NULL!=pjmp)
    {
        longjmp(*pjmp, 1);	// 跳回.
    }
    // 不能跳回.
    //fprintf(stderr, "!SIGILL!");
    abort();
}

// 尝试调用一个可能会发生SIGILL信号的函数.
//
// result: 若正常运行，就返回pfunc的返回值. 否则返回0.
// pfunc: 欲测试的函数.
// puserdata: 用户自定参数, 在调用pfunc时会传递该参数.
static int simd_try_sigill(int (*pfunc)(void*), void* puserdata)
{
    int rt = 0;
    jmp_buf myjmp;
    jmp_buf* poldjmp=NULL;	// 以前的跳回地址.
    void (*old_signal)(int) = SIG_DFL;	// 以前的信号处理函数.

    // 注册跳转.
    if (0==setjmp(myjmp))
    {
        // 登记跳回.
        poldjmp = simd_pjmpback_sigill;
        simd_pjmpback_sigill = &myjmp;

        // 注册信号处理函数.
        old_signal = signal(SIGILL, simd_catch_sigill);

        // [try]
        rt = pfunc(puserdata);
    }
    else
    {
        // [catch]
    }

    // 恢复信号处理函数.
    simd_pjmpback_sigill = poldjmp;
    signal(SIGILL, old_signal);

    return rt;
}

// 验证mmx指令是否能运行_实际指令测试.
static int	simd_try_mmx_pfunc(void* )
{
    #if defined(_M_X64) && defined(_MSC_VER) && !defined(__INTEL_COMPILER)
        // VC编译器不支持64位下的MMX.
        return 0;
    #else
        //_mm_empty();	// MMX instruction: emms
        return 1;
    #endif	// #if defined(_M_X64) && defined(_MSC_VER)
}

// 验证mmx指令是否能运行.
static int	simd_try_mmx()
{
    int rt = 0;
    #if defined(_M_X64) && defined(_MSC_VER) && !defined(__INTEL_COMPILER)
        // VC编译器不支持64位下的MMX.
    #else
        rt = simd_try_sigill(simd_try_mmx_pfunc, 0);
    #endif	// #if defined(_M_X64) && defined(_MSC_VER)
    return rt;
}

// 验证sse指令是否能运行_实际指令测试.
static int	simd_try_sse_pfunc(void* )
{
    /*int rt = 0;
    volatile __m128 xmm1 = _mm_setzero_ps();	// SSE instruction: xorps
    int* pxmm1 = (int*)&xmm1;	// 避免GCC的 -Wstrict-aliasing 警告.
    if (0==*pxmm1)	rt = 1;	// 避免Release模式编译优化时剔除_mm_setzero_ps.
    return rt;*/
    return 1;
}

// 验证sse指令是否能运行.
static int	simd_try_sse()
{
    int rt = simd_try_sigill(simd_try_sse_pfunc, 0);
    return rt;
}

// 是否支持MMX指令集.
//
// result: 返回当前运行环境是否支持MMX指令集. 非0表示支持, 0表示不支持.
// phwmmx: 返回硬件是否支持MMX指令集. 非0表示支持, 0表示不支持.
INLINE int	simd_mmx(int* phwmmx)
{
    int	rt = 0;	// result
    #ifdef CCPUID_X86
        const uint32_t	BIT_D_MMX = 0x00800000;	// bit 23
        uint32_t dwBuf[4];

        // check processor support
        cpuid(dwBuf, 1);	// Function 1: Feature Information
        if ( dwBuf[3] & BIT_D_MMX )	rt=1;
        if (NULL!=phwmmx)	*phwmmx=rt;

        // check OS support
        if ( rt>0 )
        {
            if (!simd_try_mmx()) rt=0;
        }
    #else	// #ifdef CCPUID_X86
        if (NULL!=phwmmx)	*phwmmx=rt;
    #endif	// #ifdef CCPUID_X86
    return rt;
}

// 检测SSE系列指令集的支持级别.
//
// result: 返回当前运行环境的SSE系列指令集支持级别. 详见SIMD_SSE_常数.
// phwmmx: 返回硬件的SSE系列指令集支持级别. 详见SIMD_SSE_常数.
INLINE int	simd_sse_level(int* phwsse)
{
    int	rt = SIMD_SSE_NONE;	// result
    #ifdef CCPUID_X86
        const uint32_t	BIT_D_SSE = 0x02000000;	// bit 25
        const uint32_t	BIT_D_SSE2 = 0x04000000;	// bit 26
        const uint32_t	BIT_C_SSE3 = 0x00000001;	// bit 0
        const uint32_t	BIT_C_SSSE3 = 0x00000100;	// bit 9
        const uint32_t	BIT_C_SSE41 = 0x00080000;	// bit 19
        const uint32_t	BIT_C_SSE42 = 0x00100000;	// bit 20
        uint32_t dwBuf[4];

        // check processor support
        cpuid(dwBuf, 1);	// Function 1: Feature Information
        if ( dwBuf[3] & BIT_D_SSE )
        {
            rt = SIMD_SSE_1;
            if ( dwBuf[3] & BIT_D_SSE2 )
            {
                rt = SIMD_SSE_2;
                if ( dwBuf[2] & BIT_C_SSE3 )
                {
                    rt = SIMD_SSE_3;
                    if ( dwBuf[2] & BIT_C_SSSE3 )
                    {
                        rt = SIMD_SSE_3S;
                        if ( dwBuf[2] & BIT_C_SSE41 )
                        {
                            rt = SIMD_SSE_41;
                            if ( dwBuf[2] & BIT_C_SSE42 )
                            {
                                rt = SIMD_SSE_42;
                            }
                        }
                    }
                }
            }
        }
        if (NULL!=phwsse)	*phwsse=rt;

        // check OS support
        if ( rt>0 )
        {
            if (!simd_try_sse()) rt=SIMD_SSE_NONE;
        }
    #else	// #ifdef CCPUID_X86
        if (NULL!=phwmmx)	*phwmmx=rt;
    #endif	// #ifdef CCPUID_X86
    return rt;
}

// 检测AVX系列指令集的支持级别.
//
// result: 返回操作系统的AVX系列指令集支持级别. 详见SIMD_AVX_常数.
// phwavx: 返回硬件的AVX系列指令集支持级别. 详见SIMD_AVX_常数.
INLINE int	simd_avx_level(int* phwavx)
{
    int	rt = SIMD_AVX_NONE;	// result
    #ifdef CCPUID_X86
        // check processor support
        if (0!=cpuid_field(CPUF_AVX))
        {
            rt = SIMD_AVX_1;
            if (0!=cpuid_field(CPUF_AVX2))
            {
                rt = SIMD_AVX_2;
            }
        }
        if (NULL!=phwavx)	*phwavx=rt;

        // check OS support
        if ( rt>0 )
        {
            if (0!=cpuid_field(CPUF_OSXSAVE))	// XGETBV enabled for application use.
            {
                uint32_t n = cpuid_field(CPUF_XFeatureSupportedMaskLo);	// XCR0: XFeatureSupportedMask register.
                if (6==(n&6))	// XCR0[2:1] = ‘11b’ (XMM state and YMM state are enabled by OS).
                {
                    return rt;
                }
            }
        }
        return SIMD_AVX_NONE;
    #else	// #ifdef CCPUID_X86
        if (NULL!=phwavx)	*phwavx=rt;
        return rt;
    #endif	// #ifdef CCPUID_X86
}



#if defined __cplusplus
};
#endif

#endif	// #ifndef __CCPUID_H_INCLUDED
