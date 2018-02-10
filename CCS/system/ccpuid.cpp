
#include "ccpuid.hpp"

CCPUID CCPUID::_cur;

const CPUIDFIELDDESC CCPUID::CPUFDesc[] = {
    {CPUF_LFuncStd, 0, "LFuncStd", "largest standard function."}
    ,{CPUF_Stepping, 0, "Stepping", "processor stepping."}
    ,{CPUF_BaseModel, 0, "BaseModel", "base processor model."}
    ,{CPUF_BaseFamily, 0, "BaseFamily", "base processor family."}
    ,{CPUF_ProcessorType, 0, "ProcessorType", "processor type."}
    ,{CPUF_ExtModel, 0, "ExtModel", "processor extended model."}
    ,{CPUF_ExtFamily, 0, "ExtFamily", "processor extended family."}
    ,{CPUF_BrandId8, 0, "BrandId8", "8-bit brand ID."}
    ,{CPUF_CLFlush, 0, "CLFlush", "CLFLUSH line size. (*8)"}
    ,{CPUF_MaxApicId, 0, "MaxApicId", "Maximum number of addressable IDs for logical processors in this physical package."}
    ,{CPUF_ApicId, 0, "ApicId", "Initial local APIC physical ID(8-bit)."}
    ,{CPUF_SSE3, 0, "SSE3", "Streaming SIMD Extensions 3."}
    ,{CPUF_PCLMULQDQ, 0, "PCLMULQDQ", "PCLMULQDQ instruction."}
    ,{CPUF_DTES64, 0, "DTES64", "64-bit DS Area."}
    ,{CPUF_MONITOR, 0, "MONITOR", "MONITOR/MWAIT instructions."}
    ,{CPUF_DS_CPL, 0, "DS_CPL", "CPL Qualified Debug Store."}
    ,{CPUF_VMX, 0, "VMX", "Virtual Machine Extensions."}
    ,{CPUF_SMX, 0, "SMX", "Safer Mode Extensions."}
    ,{CPUF_EIST, 0, "EIST", "Enhanced Intel SpeedStep technology."}
    ,{CPUF_TM2, 0, "TM2", "Thermal Monitor 2."}
    ,{CPUF_SSSE3, 0, "SSSE3", "Supplemental Streaming SIMD Extensions 3 (SSSE3)."}
    ,{CPUF_CNXT_ID, 0, "CNXT_ID", "L1 Context ID."}
    ,{CPUF_FMA, 0, "FMA", "supports FMA extensions using YMM state."}
    ,{CPUF_CX16, 0, "CX16", "CMPXCHG16B instruction."}
    ,{CPUF_xTPR, 0, "xTPR", "xTPR Update Control. Can disable sending Task Priority messages."}
    ,{CPUF_PDCM, 0, "PDCM", "Perfmon and Debug Capability."}
    ,{CPUF_PCID, 0, "PCID", "Process Context Identifiers."}
    ,{CPUF_DCA, 0, "DCA", "Direct Cache Access."}
    ,{CPUF_SSE41, 0, "SSE41", "SSE4.1 instructions."}
    ,{CPUF_SSE42, 0, "SSE42", "SSE4.2 instructions."}
    ,{CPUF_x2APIC, 0, "x2APIC", "Extended xAPIC Support."}
    ,{CPUF_MOVBE, 0, "MOVBE", "MOVBE Instruction."}
    ,{CPUF_POPCNT, 0, "POPCNT", "POPCNT instruction."}
    ,{CPUF_TSC_DEADLINE, 0, "TSC_DEADLINE", "Local APIC timer supports one-shot operation using a TSC deadline value."}
    ,{CPUF_AES, 0, "AES", "Advanced Encryption Standard (AES) Instructions."}
    ,{CPUF_XSAVE, 0, "XSAVE", "XSAVE (and related) instructions are supported by hardware."}
    ,{CPUF_OSXSAVE, 0, "OSXSAVE", "XSAVE (and related) instructions are enabled."}
    ,{CPUF_AVX, 0, "AVX", "AVX instructions."}
    ,{CPUF_F16C, 0, "F16C", "half-precision convert instruction support."}
    ,{CPUF_RDRAND, 0, "RDRAND", "RDRAND instruction."}
    ,{CPUF_FPU, 0, "FPU", "Floating Point Unit On-Chip."}
    ,{CPUF_VME, 0, "VME", "Virtual 8086 Mode Enhancements."}
    ,{CPUF_DE, 0, "DE", "Debugging Extensions."}
    ,{CPUF_PSE, 0, "PSE", "Page Size Extension."}
    ,{CPUF_TSC, 0, "TSC", "Time Stamp Counter."}
    ,{CPUF_MSR, 0, "MSR", "Model Specific Registers RDMSR and WRMSR Instructions."}
    ,{CPUF_PAE, 0, "PAE", "Physical Address Extension."}
    ,{CPUF_MCE, 0, "MCE", "Machine Check Exception."}
    ,{CPUF_CX8, 0, "CX8", "CMPXCHG8B instruction."}
    ,{CPUF_APIC, 0, "APIC", "APIC(Advanced Programmable Interrupt Controller) On-Chip."}
    ,{CPUF_SEP, 0, "SEP", "Fast System Call instructions, SYSENTER and SYSEXIT."}
    ,{CPUF_MTRR, 0, "MTRR", "Memory Type Range Registers."}
    ,{CPUF_PGE, 0, "PGE", "Page Global Enable."}
    ,{CPUF_MCA, 0, "MCA", "Machine-Check Architecture."}
    ,{CPUF_CMOV, 0, "CMOV", "Conditional Move Instructions."}
    ,{CPUF_PAT, 0, "PAT", "Page Attribute Table."}
    ,{CPUF_PSE36, 0, "PSE36", "36-Bit Page Size Extension."}
    ,{CPUF_PSN, 0, "PSN", "Processor Serial Number."}
    ,{CPUF_CLFSH, 0, "CLFSH", "CLFLUSH Instruction."}
    ,{CPUF_DS, 0, "DS", "Debug Store."}
    ,{CPUF_ACPI, 0, "ACPI", "Thermal Monitor and Software Controlled Clock Facilities."}
    ,{CPUF_MMX, 0, "MMX", "MMX instructions."}
    ,{CPUF_FXSR, 0, "FXSR", "FXSAVE and FXRSTOR instructions."}
    ,{CPUF_SSE, 0, "SSE", "Streaming SIMD Extensions."}
    ,{CPUF_SSE2, 0, "SSE2", "Streaming SIMD Extensions 2."}
    ,{CPUF_SS, 0, "SS", "Self Snoop."}
    ,{CPUF_HTT, 0, "HTT", "Max APIC IDs reserved field is Valid."}
    ,{CPUF_TM, 0, "TM", "Thermal Monitor."}
    ,{CPUF_PBE, 0, "PBE", "Pending Break Enable."}
    ,{CPUF_Cache_Type, 0, "Cache_Type", "Cache Type (0=Null, 1=Data, 2=Instruction, 3=Unified)."}
    ,{CPUF_Cache_Level, 0, "Cache_Level", "Cache Level (Starts at 1)."}
    ,{CPUF_CACHE_SI, 0, "CACHE_SI", "Self Initializing cache level."}
    ,{CPUF_CACHE_FA, 0, "CACHE_FA", "Fully Associative cache."}
    ,{CPUF_MaxApicIdShare, 0, "MaxApicIdShare", "Maximum number of addressable IDs for logical processors sharing this cache (plus 1 encoding)."}
    ,{CPUF_MaxApicIdCore, 0, "MaxApicIdCore", "Maximum number of addressable IDs for processor cores in the physical package (plus 1 encoding)."}
    ,{CPUF_Cache_LineSize, 0, "Cache_LineSize", "System Coherency Line Size (plus 1 encoding)."}
    ,{CPUF_Cache_Partitions, 0, "Cache_Partitions", "Physical Line partitions (plus 1 encoding)."}
    ,{CPUF_Cache_Ways, 0, "Cache_Ways", "Ways of Associativity (plus 1 encoding)."}
    ,{CPUF_Cache_Sets, 0, "Cache_Sets", "Number of Sets (plus 1 encoding)."}
    ,{CPUF_CACHE_INVD, 0, "CACHE_INVD", "WBINVD/INVD behavior on lower level caches."}
    ,{CPUF_CACHE_INCLUSIVENESS, 0, "CACHE_INCLUSIVENESS", "Cache is inclusive of lower cache levels."}
    ,{CPUF_CACHE_COMPLEXINDEX, 0, "CACHE_COMPLEXINDEX", "Complex Cache Indexing."}
    ,{CPUF_MonLineSizeMin, 0, "MonLineSizeMin", "Smallest monitor line size in bytes."}
    ,{CPUF_MonLineSizeMax, 0, "MonLineSizeMax", "Largest monitor-line size in bytes."}
    ,{CPUF_EMX, 0, "EMX", "Enumerate MONITOR/MWAIT extensions."}
    ,{CPUF_IBE, 0, "IBE", "Interrupt Break-Event."}
    ,{CPUF_MWAIT_Number_C0, 0, "MWAIT_Number_C0", "Number of C0 sub C-states supported using MWAIT."}
    ,{CPUF_MWAIT_Number_C1, 0, "MWAIT_Number_C1", "Number of C1 sub C-states supported using MWAIT."}
    ,{CPUF_MWAIT_Number_C2, 0, "MWAIT_Number_C2", "Number of C2 sub C-states supported using MWAIT."}
    ,{CPUF_MWAIT_Number_C3, 0, "MWAIT_Number_C3", "Number of C3 sub C-states supported using MWAIT."}
    ,{CPUF_MWAIT_Number_C4, 0, "MWAIT_Number_C4", "Number of C4 sub C-states supported using MWAIT."}
    ,{CPUF_DTS, 0, "DTS", "Digital Thermal Sensor."}
    ,{CPUF_TURBO_BOOST, 0, "TURBO_BOOST", "Intel Turbo Boost Technology."}
    ,{CPUF_ARAT, 0, "ARAT", "Always Running APIC Timer."}
    ,{CPUF_PLN, 0, "PLN", "Power Limit Notification."}
    ,{CPUF_ECMD, 0, "ECMD", "Extended Clock Modulation Duty."}
    ,{CPUF_PTM, 0, "PTM", "Package Thermal Management."}
    ,{CPUF_DTS_ITs, 0, "DTS_ITs", "Number of Interrupt Thresholds in Digital Thermal Sensor."}
    ,{CPUF_PERF, 0, "PERF", "Hardware Coordination Feedback Capability."}
    ,{CPUF_ACNT2, 0, "ACNT2", "ACNT2 Capability."}
    ,{CPUF_ENERGY_PERF_BIAS, 0, "ENERGY_PERF_BIAS", "Performance-Energy Bias capability."}
    ,{CPUF_Max07Subleaf, 0, "Max07Subleaf", "Reports the maximum supported leaf 7 sub-leaf."}
    ,{CPUF_FSGSBASE, 0, "FSGSBASE", "Supports RDFSBASE/RDGSBASE/WRFSBASE/WRGSBASE."}
    ,{CPUF_TSC_ADJUST, 0, "TSC_ADJUST", "IA32_TSC_ADJUST MSR is supported if 1."}
    ,{CPUF_BMI1, 0, "BMI1", "The first group of advanced bit manipulation extensions (ANDN, BEXTR, BLSI, BLSMK, BLSR, TZCNT)."}
    ,{CPUF_HLE, 0, "HLE", "Hardware Lock Elision."}
    ,{CPUF_AVX2, 0, "AVX2", "AVX2 instructions."}
    ,{CPUF_SMEP, 0, "SMEP", "Supervisor Mode Execution Protection."}
    ,{CPUF_BMI2, 0, "BMI2", "The second group of advanced bit manipulation extensions (BZHI, MULX, PDEP, PEXT, RORX, SARX, SHLX, SHRX)."}
    ,{CPUF_ERMS, 0, "ERMS", "Supports Enhanced REP MOVSB/STOSB."}
    ,{CPUF_INVPCID, 0, "INVPCID", "Invalidate Processor Context ID."}
    ,{CPUF_RTM, 0, "RTM", "Restricted Transactional Memory."}
    ,{CPUF_RDSEED, 0, "RDSEED", "RDSEED instruction."}
    ,{CPUF_ADX, 0, "ADX", "ADCX and ADOX instructions."}
    ,{CPUF_SMAP, 0, "SMAP", "Supervisor Mode Access Prevention."}
    ,{CPUF_PLATFORM_DCA_CAP, 0, "PLATFORM_DCA_CAP", "Value of PLATFORM_DCA_CAP MSR Bits [31:0] (Offset 1F8h)."}
    ,{CPUF_APM_Version, 0, "APM_Version", "Version ID of architectural performance monitoring."}
    ,{CPUF_APM_Counters, 0, "APM_Counters", "Number of general-purpose performance monitoring counters per logical processor."}
    ,{CPUF_APM_Bits, 0, "APM_Bits", "Bit width of general-purpose, performance monitoring counters."}
    ,{CPUF_APM_Length, 0, "APM_Length", "Length of EBX bit vector to enumerate architectural performance monitoring events."}
    ,{CPUF_APM_CC, 0, "APM_CC", "Core cycle event not available if 1."}
    ,{CPUF_APM_IR, 0, "APM_IR", "Instruction retired event not available if 1."}
    ,{CPUF_APM_RC, 0, "APM_RC", "Reference cycles event not available if 1."}
    ,{CPUF_APM_LLCR, 0, "APM_LLCR", "Last-level cache reference event not available if 1."}
    ,{CPUF_APM_LLCM, 0, "APM_LLCM", "Last-level cache misses event not available if 1."}
    ,{CPUF_APM_BIR, 0, "APM_BIR", "Branch instruction retired event not available if 1."}
    ,{CPUF_APM_BMR, 0, "APM_BMR", "Branch mispredict retired event not available if 1."}
    ,{CPUF_APM_FC_Number, 0, "APM_FC_Number", "Number of fixed-function performance counters."}
    ,{CPUF_APM_FC_Bits, 0, "APM_FC_Bits", "Bit width of fixed-function performance counters."}
    ,{CPUF_Topology_Bits, 0, "Topology_Bits", "Number of bits to shift right on x2APIC ID to get a unique topology ID of the next level type."}
    ,{CPUF_Topology_Number, 0, "Topology_Number", "Number of factory-configured logical processors at this level."}
    ,{CPUF_Topology_Level, 0, "Topology_Level", "Level number. Same value in ECX input."}
    ,{CPUF_Topology_Type, 0, "Topology_Type", "Level Type (0=Invalid, 1=Thread, 2=Core)."}
    ,{CPUF_X2APICID, 0, "X2APICID", "x2APIC ID."}
    ,{CPUF_XFeatureSupportedMaskLo, 0, "XFeatureSupportedMaskLo", "The lower 32 bits of XCR0(XFEATURE_ENABLED_MASK register)."}
    ,{CPUF_XFeatureEnabledSizeMax, 0, "XFeatureEnabledSizeMax", "Size in bytes of XSAVE/XRSTOR area for the currently enabled features in XCR0."}
    ,{CPUF_XFeatureSupportedSizeMax, 0, "XFeatureSupportedSizeMax", "Size in bytes of XSAVE/XRSTOR area for all features that the core supports."}
    ,{CPUF_XFeatureSupportedMaskHi, 0, "XFeatureSupportedMaskHi", "The upper 32 bits of XCR0(XFEATURE_ENABLED_MASK register)."}
    ,{CPUF_XSAVEOPT, 0, "XSAVEOPT", "XSAVEOPT is available."}
    ,{CPUF_YmmSaveStateSize, 0, "YmmSaveStateSize", "YMM save state byte size."}
    ,{CPUF_YmmSaveStateOffset, 0, "YmmSaveStateOffset", "YMM save state byte offset."}
    ,{CPUF_LwpSaveStateSize, 0, "LwpSaveStateSize", "LWP save state byte size."}
    ,{CPUF_LwpSaveStateOffset, 0, "LwpSaveStateOffset", "LWP save state byte offset."}
    ,{CPUF_LFuncExt, 0, "LFuncExt", "Largest extended function."}
    ,{CPUF_BrandId16, 0, "BrandId16", "16-bit Brand ID."}
    ,{CPUF_PkgType, 0, "PkgType", "Package type (Family[7:0] >= 10h)."}
    ,{CPUF_LahfSahf, 0, "LahfSahf", "LAHF and SAHF instruction support in 64-bit mode."}
    ,{CPUF_CmpLegacy, 0, "CmpLegacy", "core multi-processing legacy mode."}
    ,{CPUF_SVM, 0, "SVM", "secure virtual machine."}
    ,{CPUF_ExtApicSpace, 0, "ExtApicSpace", "extended APIC space."}
    ,{CPUF_AltMovCr8, 0, "AltMovCr8", "LOCK MOV CR0 means MOV CR8."}
    ,{CPUF_ABM, 0, "ABM", "advanced bit manipulation (LZCNT)."}
    ,{CPUF_SSE4A, 0, "SSE4A", "SSE4A instructions."}
    ,{CPUF_MisAlignSse, 0, "MisAlignSse", "misaligned SSE mode."}
    ,{CPUF_3DNowPrefetch, 0, "3DNowPrefetch", "PREFETCH and PREFETCHW instruction support."}
    ,{CPUF_OSVW, 0, "OSVW", "OS visible workaround."}
    ,{CPUF_IBS, 0, "IBS", "instruction based sampling."}
    ,{CPUF_XOP, 0, "XOP", "extended operation support."}
    ,{CPUF_SKINIT, 0, "SKINIT", "SKINIT and STGI are supported, independent of the value of MSRC000_0080[SVME]."}
    ,{CPUF_WDT, 0, "WDT", "watchdog timer support."}
    ,{CPUF_LWP, 0, "LWP", "lightweight profiling support."}
    ,{CPUF_FMA4, 0, "FMA4", "4-operand FMA instruction support."}
    ,{CPUF_BIT_NODEID, 0, "BIT_NODEID", "Indicates support for MSRC001_100C[NodeId, NodesPerProcessor]."}
    ,{CPUF_TBM, 0, "TBM", "Trailing bit manipulation instruction support."}
    ,{CPUF_TopologyExtensions, 0, "TopologyExtensions", "Topology extensions support."}
    ,{CPUF_SYSCALL, 0, "SYSCALL", "SYSCALL and SYSRET instructions."}
    ,{CPUF_XD, 0, "XD", "Execution Disable Bit."}
    ,{CPUF_MmxExt, 0, "MmxExt", "AMD extensions to MMX instructions."}
    ,{CPUF_FFXSR, 0, "FFXSR", "FXSAVE and FXRSTOR instruction optimizations."}
    ,{CPUF_Page1GB, 0, "Page1GB", "1-GB large page support."}
    ,{CPUF_RDTSCP, 0, "RDTSCP", "RDTSCP and TSC_AUX."}
    ,{CPUF_LM, 0, "LM", "64-bit long mode.(x86-64)"}
    ,{CPUF_3DNowExt, 0, "3DNowExt", "AMD extensions to 3DNow! instructions."}
    ,{CPUF_3DNow, 0, "3DNow", "3DNow! instructions."}
    ,{CPUF_L1ITlb2and4MSize, 0, "L1ITlb2and4MSize", "Instruction TLB number of entries for 2-MB and 4-MB pages."}
    ,{CPUF_L1ITlb2and4MAssoc, 0, "L1ITlb2and4MAssoc", "Instruction TLB associativity for 2-MB and 4-MB pages."}
    ,{CPUF_L1DTlb2and4MSize, 0, "L1DTlb2and4MSize", "Data TLB number of entries for 2-MB and 4-MB pages."}
    ,{CPUF_L1DTlb2and4MAssoc, 0, "L1DTlb2and4MAssoc", "Data TLB associativity for 2-MB and 4-MB pages."}
    ,{CPUF_L1ITlb4KSize, 0, "L1ITlb4KSize", "Instruction TLB number of entries for 4 KB pages."}
    ,{CPUF_L1ITlb4KAssoc, 0, "L1ITlb4KAssoc", "Instruction TLB associativity for 4KB pages."}
    ,{CPUF_L1DTlb4KSize, 0, "L1DTlb4KSize", "Data TLB number of entries for 4 KB pages."}
    ,{CPUF_L1DTlb4KAssoc, 0, "L1DTlb4KAssoc", "Data TLB associativity for 4 KB pages."}
    ,{CPUF_L1DcLineSize, 0, "L1DcLineSize", "L1 data cache line size in bytes."}
    ,{CPUF_L1DcLinesPerTag, 0, "L1DcLinesPerTag", "L1 data cache lines per tag."}
    ,{CPUF_L1DcAssoc, 0, "L1DcAssoc", "L1 data cache associativity."}
    ,{CPUF_L1DcSize, 0, "L1DcSize", "L1 data cache size in KB."}
    ,{CPUF_L1IcLineSize, 0, "L1IcLineSize", "L1 instruction cache line size in bytes"}
    ,{CPUF_L1IcLinesPerTag, 0, "L1IcLinesPerTag", "L1 instruction cache lines per tag."}
    ,{CPUF_L1IcAssoc, 0, "L1IcAssoc", "L1 instruction cache associativity."}
    ,{CPUF_L1IcSize, 0, "L1IcSize", "L1 instruction cache size KB."}
    ,{CPUF_L2ITlb2and4MSize, 0, "L2ITlb2and4MSize", "L2 instruction TLB number of entries for 2 MB and 4 MB pages."}
    ,{CPUF_L2ITlb2and4MAssoc, 0, "L2ITlb2and4MAssoc", "L2 instruction TLB associativity for 2 MB and 4 MB pages."}
    ,{CPUF_L2DTlb2and4MSize, 0, "L2DTlb2and4MSize", "L2 data TLB number of entries for 2 MB and 4 MB pages."}
    ,{CPUF_L2DTlb2and4MAssoc, 0, "L2DTlb2and4MAssoc", "L2 data TLB associativity for 2 MB and 4 MB pages."}
    ,{CPUF_L2ITlb4KSize, 0, "L2ITlb4KSize", "L2 instruction TLB number of entries for 4 KB pages."}
    ,{CPUF_L2ITlb4KAssoc, 0, "L2ITlb4KAssoc", "L2 instruction TLB associativity for 4 KB pages."}
    ,{CPUF_L2DTlb4KSize, 0, "L2DTlb4KSize", "L2 data TLB number of entries for 4 KB pages."}
    ,{CPUF_L2DTlb4KAssoc, 0, "L2DTlb4KAssoc", "L2 data TLB associativity for 4 KB pages."}
    ,{CPUF_L2LineSize, 0, "L2LineSize", "L2 cache line size in bytes."}
    ,{CPUF_L2LinesPerTag, 0, "L2LinesPerTag", "L2 cache lines per tag."}
    ,{CPUF_L2Assoc, 0, "L2Assoc", "L2 cache associativity."}
    ,{CPUF_L2Size, 0, "L2Size", "L2 cache size in KB."}
    ,{CPUF_L3LineSize, 0, "L3LineSize", "L3 cache line size in bytes."}
    ,{CPUF_L3LinesPerTag, 0, "L3LinesPerTag", "L3 cache lines per tag."}
    ,{CPUF_L3Assoc, 0, "L3Assoc", "L3 cache associativity."}
    ,{CPUF_L3Size, 0, "L3Size", "L3 cache size."}
    ,{CPUF_TS, 0, "TS", "Temperature sensor."}
    ,{CPUF_FID, 0, "FID", "Frequency ID control."}
    ,{CPUF_VID, 0, "VID", "Voltage ID control."}
    ,{CPUF_TTP, 0, "TTP", "THERMTRIP."}
    ,{CPUF_HTC, 0, "HTC", "TM: Hardware thermal control (HTC)."}
    ,{CPUF_100MHzSteps, 0, "100MHzSteps", "100 MHz multiplier Control."}
    ,{CPUF_HwPstate, 0, "HwPstate", "Hardware P-state control."}
    ,{CPUF_TscInvariant, 0, "TscInvariant", "TSC invariant."}
    ,{CPUF_CPB, 0, "CPB", "Core performance boost."}
    ,{CPUF_EffFreqRO, 0, "EffFreqRO", "Read-only effective frequency interface."}
    ,{CPUF_PhysAddrSize, 0, "PhysAddrSize", "Maximum physical byte address size in bits."}
    ,{CPUF_LinAddrSize, 0, "LinAddrSize", "Maximum linear byte address size in bits."}
    ,{CPUF_GuestPhysAddrSize, 0, "GuestPhysAddrSize", "Maximum guest physical byte address size in bits."}
    ,{CPUF_NC, 0, "NC", "number of physical cores - 1."}
    ,{CPUF_ApicIdCoreIdSize, 0, "ApicIdCoreIdSize", "APIC ID size. The number of bits in the initial APIC20[ApicId] value that indicate core ID within a processor."}
    ,{CPUF_SvmRev, 0, "SvmRev", "SVM revision."}
    ,{CPUF_NASID, 0, "NASID", "number of address space identifiers (ASID)."}
    ,{CPUF_NP, 0, "NP", "Nested paging."}
    ,{CPUF_LbrVirt, 0, "LbrVirt", "LBR virtualization."}
    ,{CPUF_SVML, 0, "SVML", "SVM lock. Indicates support for SVM-Lock."}
    ,{CPUF_NRIPS, 0, "NRIPS", "NRIP save. Indicates support for NRIP save on #VMEXIT."}
    ,{CPUF_TscRateMsr, 0, "TscRateMsr", "MSR based TSC rate control."}
    ,{CPUF_VmcbClean, 0, "VmcbClean", "VMCB clean bits. Indicates support for VMCB clean bits."}
    ,{CPUF_FlushByAsid, 0, "FlushByAsid", "Flush by ASID."}
    ,{CPUF_DecodeAssists, 0, "DecodeAssists", "Decode assists."}
    ,{CPUF_PauseFilter, 0, "PauseFilter", "Pause intercept filter."}
    ,{CPUF_PauseFilterThreshold, 0, "PauseFilterThreshold", "PAUSE filter threshold."}
    ,{CPUF_L1ITlb1GSize, 0, "L1ITlb1GSize", "L1 instruction TLB number of entries for 1 GB pages."}
    ,{CPUF_L1ITlb1GAssoc, 0, "L1ITlb1GAssoc", "L1 instruction TLB associativity for 1 GB pages."}
    ,{CPUF_L1DTlb1GSize, 0, "L1DTlb1GSize", "L1 data TLB number of entries for 1 GB pages."}
    ,{CPUF_L1DTlb1GAssoc, 0, "L1DTlb1GAssoc", "L1 data TLB associativity for 1 GB pages."}
    ,{CPUF_L2ITlb1GSize, 0, "L2ITlb1GSize", "L2 instruction TLB number of entries for 1 GB pages."}
    ,{CPUF_L2ITlb1GAssoc, 0, "L2ITlb1GAssoc", "L2 instruction TLB associativity for 1 GB pages."}
    ,{CPUF_L2DTlb1GSize, 0, "L2DTlb1GSize", "L2 data TLB number of entries for 1 GB pages."}
    ,{CPUF_L2DTlb1GAssoc, 0, "L2DTlb1GAssoc", "L2 data TLB associativity for 1 GB pages."}
    ,{CPUF_FP128, 0, "FP128", "128-bit SSE (multimedia) instructions are executed with full-width internal operations and pipelines rather than decomposing them into internal 64-bit suboperations."}
    ,{CPUF_MOVU, 0, "MOVU", "MOVU SSE (multimedia) instructions are more efficient and should be preferred to SSE(multimedia) MOVL/MOVH. MOVUPS is more efficient than MOVLPS/MOVHPS."}
    ,{CPUF_IBSFFV, 0, "IBSFFV", "IBS feature flags valid."}
    ,{CPUF_FetchSam, 0, "FetchSam", "IBS fetch sampling supported."}
    ,{CPUF_OpSam, 0, "OpSam", "IBS execution sampling supported."}
    ,{CPUF_RdWrOpCnt, 0, "RdWrOpCnt", "Read write of op counter supported."}
    ,{CPUF_OpCnt, 0, "OpCnt", "Op counting mode supported."}
    ,{CPUF_BrnTrgt, 0, "BrnTrgt", "Branch target address reporting supported."}
    ,{CPUF_OpCntExt, 0, "OpCntExt", "IbsOpCurCnt and IbsOpMaxCnt extend by 7 bits."}
    ,{CPUF_RipInvalidChk, 0, "RipInvalidChk", "Invalid RIP indication supported."}
    ,{CPUF_LwpAvail, 0, "LwpAvail", "LWP available."}
    ,{CPUF_LwpVAL, 0, "LwpVAL", "LWPVAL instruction available."}
    ,{CPUF_LwpIRE, 0, "LwpIRE", "instructions retired event available."}
    ,{CPUF_LwpBRE, 0, "LwpBRE", "branch retired event available."}
    ,{CPUF_LwpDME, 0, "LwpDME", "DC miss event available."}
    ,{CPUF_LwpCNH, 0, "LwpCNH", "core clocks not halted event available."}
    ,{CPUF_LwpRNH, 0, "LwpRNH", "core reference clocks not halted event available."}
    ,{CPUF_LwpInt, 0, "LwpInt", "interrupt on threshold overflow available."}
    ,{CPUF_LwpCbSize, 0, "LwpCbSize", "control block size. Size in bytes of the LWPCB."}
    ,{CPUF_LwpEventSize, 0, "LwpEventSize", "event record size. Size in bytes of an event record in the LWP event ring buffer."}
    ,{CPUF_LwpMaxEvents, 0, "LwpMaxEvents", "maximum EventId. Maximum EventId value that is supported."}
    ,{CPUF_LwpEventOffset, 0, "LwpEventOffset", "offset to the EventInterval1 field. Offset from the start of the LWPCB to the EventInterval1 field."}
    ,{CPUF_LwpLatencyMax, 0, "LwpLatencyMax", "latency counter bit size. Size in bits of the cache latency counters."}
    ,{CPUF_LwpDataAddress, 0, "LwpDataAddress", "data cache miss address valid."}
    ,{CPUF_LwpLatencyRnd, 0, "LwpLatencyRnd", "amount cache latency is rounded."}
    ,{CPUF_LwpVersion, 0, "LwpVersion", "version. Version of LWP implementation."}
    ,{CPUF_LwpMinBufferSize, 0, "LwpMinBufferSize", "event ring buffer size. Minimum size of the LWP event ring buffer, in units of 32 event records."}
    ,{CPUF_LwpBranchPrediction, 0, "LwpBranchPrediction", "branch prediction filtering supported."}
    ,{CPUF_LwpIpFiltering, 0, "LwpIpFiltering", "IP filtering supported."}
    ,{CPUF_LwpCacheLevels, 0, "LwpCacheLevels", "cache level filtering supported."}
    ,{CPUF_LwpCacheLatency, 0, "LwpCacheLatency", "cache latency filtering supported."}
    ,{CPUF_D_LwpAvail, 0, "D_LwpAvail", "lightweight profiling supported."}
    ,{CPUF_D_LwpVAL, 0, "D_LwpVAL", "LWPVAL instruction supported."}
    ,{CPUF_D_LwpIRE, 0, "D_LwpIRE", "instructions retired event supported."}
    ,{CPUF_D_LwpBRE, 0, "D_LwpBRE", "branch retired event supported."}
    ,{CPUF_D_LwpDME, 0, "D_LwpDME", "DC miss event supported."}
    ,{CPUF_D_LwpCNH, 0, "D_LwpCNH", "core clocks not halted event supported."}
    ,{CPUF_D_LwpRNH, 0, "D_LwpRNH", "core reference clocks not halted event supported."}
    ,{CPUF_D_LwpInt, 0, "D_LwpInt", "interrupt on threshold overflow supported."}
    ,{CPUF_CacheType, 0, "CacheType", "Cache Type (0=Null, 1=Data, 2=Instruction, 3=Unified)."}
    ,{CPUF_CacheLevel, 0, "CacheLevel", "Cache Level (Starts at 1)."}
    ,{CPUF_SelfInitialization, 0, "SelfInitialization", "Self Initializing cache level."}
    ,{CPUF_FullyAssociative, 0, "FullyAssociative", "Fully Associative cache."}
    ,{CPUF_NumSharingCache, 0, "NumSharingCache", "Number of cores sharing cache. The number of cores sharing this cache is NumSharingCache+1."}
    ,{CPUF_CacheLineSize, 0, "CacheLineSize", "Cache line size in bytes (plus 1 encoding)."}
    ,{CPUF_CachePhysPartitions, 0, "CachePhysPartitions", "Cache physical line partitions (plus 1 encoding)."}
    ,{CPUF_CacheNumWays, 0, "CacheNumWays", "Cache number of ways (plus 1 encoding)."}
    ,{CPUF_CacheNumSets, 0, "CacheNumSets", "Cache number of sets (plus 1 encoding)."}
    ,{CPUF_WBINVD, 0, "WBINVD", "Write-Back Invalidate/Invalidate (WBINVD/INVD)."}
    ,{CPUF_CacheInclusive, 0, "CacheInclusive", "Cache inclusive."}
    ,{CPUF_ExtendedApicId, 0, "ExtendedApicId", "extended APIC ID."}
    ,{CPUF_ComputeUnitId, 0, "ComputeUnitId", "compute unit ID. Identifies the processor compute unit ID."}
    ,{CPUF_CoresPerComputeUnit, 0, "CoresPerComputeUnit", "cores per compute unit. The number of cores per compute unit is CoresPerComputeUnit+1."}
    ,{CPUF_NodeId, 0, "NodeId", "Specifies the node ID."}
    ,{CPUF_NodesPerProcessor, 0, "NodesPerProcessor", "Specifies the number of nodes per processor."}
    };

const char*	CCPUID::CacheDesc[] = {
    "Null descriptor, this byte contains no information"
    ,"Instruction TLB: 4 KByte pages, 4-way set associative, 32 entries"
    ,"Instruction TLB: 4 MByte pages, fully associative, 2 entries"
    ,"Data TLB: 4 KByte pages, 4-way set associative, 64 entries"
    ,"Data TLB: 4 MByte pages, 4-way set associative, 8 entries"
    ,"Data TLB1: 4 MByte pages, 4-way set associative, 32 entries"
    ,"1st-level instruction cache: 8 KBytes, 4-way set associative, 32 byte line size"
    ,""
    ,"1st-level instruction cache: 16 KBytes, 4-way set associative, 32 byte line size"
    ,"1st-level instruction cache: 32KBytes, 4-way set associative, 64 byte line size"
    ,"1st-level data cache: 8 KBytes, 2-way set associative, 32 byte line size"
    ,"Instruction TLB: 4 MByte pages, 4-way set associative, 4 entries"
    ,"1st-level data cache: 16 KBytes, 4-way set associative, 32 byte line size"
    ,"1st-level data cache: 16 KBytes, 4-way set associative, 64 byte line size"
    ,"1st-level data cache: 24 KBytes, 6-way set associative, 64 byte line size"
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,"2nd-level cache: 256 KBytes, 8-way set associative, 64 byte line size"
    ,"3rd-level cache: 512 KBytes, 4-way set associative, 64 byte line size, 2 lines per sector"
    ,"3rd-level cache: 1 MBytes, 8-way set associative, 64 byte line size, 2 lines per sector"
    ,""
    ,"3rd-level cache: 2 MBytes, 8-way set associative, 64 byte line size, 2 lines per sector"
    ,""
    ,""
    ,""
    ,"3rd-level cache: 4 MBytes, 8-way set associative, 64 byte line size, 2 lines per sector"
    ,""
    ,""
    ,"1st-level data cache: 32 KBytes, 8-way set associative, 64 byte line size"
    ,""
    ,""
    ,""
    ,"1st-level instruction cache: 32 KBytes, 8-way set associative, 64 byte line size"
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,"No 2nd-level cache or, if processor contains a valid 2nd-level cache, no 3rd-level cache"
    ,"2nd-level cache: 128 KBytes, 4-way set associative, 32 byte line size"
    ,"2nd-level cache: 256 KBytes, 4-way set associative, 32 byte line size"
    ,"2nd-level cache: 512 KBytes, 4-way set associative, 32 byte line size"
    ,"2nd-level cache: 1 MByte, 4-way set associative, 32 byte line size"
    ,"2nd-level cache: 2 MByte, 4-way set associative, 32 byte line size"
    ,"3rd-level cache: 4 MByte, 4-way set associative, 64 byte line size"
    ,"3rd-level cache: 8 MByte, 8-way set associative, 64 byte line size"
    ,"2nd-level cache: 3MByte, 12-way set associative, 64 byte line size"
    ,"3rd-level cache: 4MB, 16-way set associative, 64-byte line size (Intel Xeon processor MP, Family 0FH, Model 06H); 2nd-level cache: 4 MByte, 16-way set associative, 64 byte line size"
    ,"3rd-level cache: 6MByte, 12-way set associative, 64 byte line size"
    ,"3rd-level cache: 8MByte, 16-way set associative, 64 byte line size"
    ,"3rd-level cache: 12MByte, 12-way set associative, 64 byte line size"
    ,"3rd-level cache: 16MByte, 16-way set associative, 64 byte line size"
    ,"2nd-level cache: 6MByte, 24-way set associative, 64 byte line size"
    ,"Instruction TLB: 4 KByte pages, 32 entries"
    ,"Instruction TLB: 4 KByte and 2-MByte or 4-MByte pages, 64 entries"
    ,"Instruction TLB: 4 KByte and 2-MByte or 4-MByte pages, 128 entries"
    ,"Instruction TLB: 4 KByte and 2-MByte or 4-MByte pages, 256 entries"
    ,""
    ,""
    ,"Instruction TLB: 2-MByte or 4-MByte pages, fully associative, 7 entries"
    ,"Data TLB0: 4 MByte pages, 4-way set associative, 16 entries"
    ,"Data TLB0: 4 KByte pages, 4-way associative, 16 entries"
    ,"Data TLB0: 4 KByte pages, fully associative, 16 entries"
    ,"Data TLB0: 2-MByte or 4 MByte pages, 4-way set associative, 32 entries"
    ,"Data TLB: 4 KByte and 4 MByte pages, 64 entries"
    ,"Data TLB: 4 KByte and 4 MByte pages,128 entries"
    ,"Data TLB: 4 KByte and 4 MByte pages,256 entries"
    ,""
    ,""
    ,""
    ,"1st-level data cache: 16 KByte, 8-way set associative, 64 byte line size"
    ,""
    ,""
    ,""
    ,""
    ,""
    ,"1st-level data cache: 8 KByte, 4-way set associative, 64 byte line size"
    ,"1st-level data cache: 16 KByte, 4-way set associative, 64 byte line size"
    ,"1st-level data cache: 32 KByte, 4-way set associative, 64 byte line size"
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,"Trace cache: 12 K-μop, 8-way set associative"
    ,"Trace cache: 16 K-μop, 8-way set associative"
    ,"Trace cache: 32 K-μop, 8-way set associative"
    ,""
    ,""
    ,""
    ,"Instruction TLB: 2M/4M pages, fully associative, 8 entries"
    ,""
    ,"2nd-level cache: 1 MByte, 4-way set associative, 64byte line size"
    ,"2nd-level cache: 128 KByte, 8-way set associative, 64 byte line size, 2 lines per sector"
    ,"2nd-level cache: 256 KByte, 8-way set associative, 64 byte line size, 2 lines per sector"
    ,"2nd-level cache: 512 KByte, 8-way set associative, 64 byte line size, 2 lines per sector"
    ,"2nd-level cache: 1 MByte, 8-way set associative, 64 byte line size, 2 lines per sector"
    ,"2nd-level cache: 2 MByte, 8-way set associative, 64byte line size"
    ,""
    ,"2nd-level cache: 512 KByte, 2-way set associative, 64-byte line size"
    ,"2nd-level cache: 512 KByte, 8-way set associative, 64-byte line size"
    ,""
    ,"2nd-level cache: 256 KByte, 8-way set associative, 32 byte line size"
    ,"2nd-level cache: 512 KByte, 8-way set associative, 32 byte line size"
    ,"2nd-level cache: 1 MByte, 8-way set associative, 32 byte line size"
    ,"2nd-level cache: 2 MByte, 8-way set associative, 32 byte line size"
    ,"2nd-level cache: 512 KByte, 4-way set associative, 64 byte line size"
    ,"2nd-level cache: 1 MByte, 8-way set associative, 64 byte line size"
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,"Instruction TLB: 4 KByte pages, 4-way set associative, 128 entries"
    ,"Instruction TLB: 2M pages, 4-way, 8 entries or 4M pages, 4-way, 4 entries"
    ,"Instruction TLB: 4KByte pages, 4-way set associative, 64 entries"
    ,"Data TLB: 4 KByte pages, 4-way set associative, 128 entries"
    ,"Data TLB1: 4 KByte pages, 4-way associative, 256 entries"
    ,""
    ,""
    ,""
    ,""
    ,""
    ,"Data TLB1: 4 KByte pages, 4-way associative, 64 entries"
    ,""
    ,""
    ,""
    ,""
    ,""
    ,"Data TLB: 4 KByte and 4 MByte pages, 4-way associative, 8 entries"
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,"Shared 2nd-Level TLB: 4 KByte pages, 4-way associative, 512 entries"
    ,""
    ,""
    ,""
    ,""
    ,""
    ,"3rd-level cache: 512 KByte, 4-way set associative, 64 byte line size"
    ,"3rd-level cache: 1 MByte, 4-way set associative, 64 byte line size"
    ,"3rd-level cache: 2 MByte, 4-way set associative, 64 byte line size"
    ,""
    ,""
    ,""
    ,"3rd-level cache: 1 MByte, 8-way set associative, 64 byte line size"
    ,"3rd-level cache: 2 MByte, 8-way set associative, 64 byte line size"
    ,"3rd-level cache: 4 MByte, 8-way set associative, 64 byte line size"
    ,""
    ,""
    ,""
    ,"3rd-level cache: 1.5 MByte, 12-way set associative, 64 byte line size"
    ,"3rd-level cache: 3 MByte, 12-way set associative, 64 byte line size"
    ,"3rd-level cache: 6 MByte, 12-way set associative, 64 byte line size"
    ,""
    ,""
    ,""
    ,"3rd-level cache: 2 MByte, 16-way set associative, 64 byte line size"
    ,"3rd-level cache: 4 MByte, 16-way set associative, 64 byte line size"
    ,"3rd-level cache: 8 MByte, 16-way set associative, 64 byte line size"
    ,""
    ,""
    ,""
    ,""
    ,""
    ,"3rd-level cache: 12MByte, 24-way set associative, 64 byte line size"
    ,"3rd-level cache: 18MByte, 24-way set associative, 64 byte line size"
    ,"3rd-level cache: 24MByte, 24-way set associative, 64 byte line size"
    ,""
    ,""
    ,""
    ,"64-Byte prefetching"
    ,"128-Byte prefetching"
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,""
    ,"CPUID leaf 2 does not report cache descriptor information, use CPUID leaf 4 to query cache parameters"
};

const char*	CCPUID::SseNames[] = {
    "None",
    "SSE",
    "SSE2",
    "SSE3",
    "SSSE3",
    "SSE4.1",
    "SSE4.2",
};

const char*	CCPUID::AvxNames[] = {
    "None",
    "AVX",
    "AVX2"
};

CCPUID::CCPUID()
    :_InfoCount(0), _LFuncStd(0), _LFuncExt(0), _BrandTrim(0)
{
    _Vendor[0] = '\0';
    _Brand[0] = '\0';
}

// 刷新信息.在Info数组中追加一项.
void CCPUID::RefreshInfo_Put(uint32_t fid, uint32_t fidsub, uint32_t CPUInfo[4])
{
    if (_InfoCount>=MAX_CPUIDINFO)	return;
    Info[_InfoCount].fid = fid;
    Info[_InfoCount].fidsub = fidsub;
    Info[_InfoCount].dw[0] = CPUInfo[0];
    Info[_InfoCount].dw[1] = CPUInfo[1];
    Info[_InfoCount].dw[2] = CPUInfo[2];
    Info[_InfoCount].dw[3] = CPUInfo[3];
    ++_InfoCount;
}

// 刷新信息.
void CCPUID::RefreshInfo()
{
    uint32_t CPUInfo[4];
    uint32_t nmax;
    uint32_t i,j;

    // == 将CPUID信息保存到Info数组 ==
    _InfoCount = 0;

    // 标准功能.
    cpuid(CPUInfo, 0);
    RefreshInfo_Put(0, 0, CPUInfo);
    nmax = CPUInfo[0];	// CPUID(0).EAX[31:0]=LFuncStd
    _LFuncStd = nmax;
    for(i=1; i<=nmax; ++i)
    {
        cpu_idex(CPUInfo, i, 0);
        RefreshInfo_Put(i, 0, CPUInfo);
        // fidsub
        if (0x4==i)	// Deterministic Cache Parameters (Function 04h)
        {
            j=1;
            while(true)
            {
                cpu_idex(CPUInfo, i, j);
                if (0==(CPUInfo[0]&0x1F))	break;	// EAX[4:0]=Cache_Type
                RefreshInfo_Put(i, j, CPUInfo);
                // next
                ++j;
            }
        }
        else if (0xB==i)	// x2APIC Features / Processor Topology (Function 0Bh)
        {
            j=1;
            while(true)
            {
                cpu_idex(CPUInfo, i, j);
                if (0==CPUInfo[0] && 0==CPUInfo[1])	break;	// until EAX=0 and EBX=0
                RefreshInfo_Put(i, j, CPUInfo);
                // next
                ++j;
            }
        }
        else if (0xD==i)	// XSAVE Features (Function 0Dh)
        {
            // fidsub = 1
            j=1;
            cpu_idex(CPUInfo, i, j);
            RefreshInfo_Put(i, j, CPUInfo);
            // fidsub >= 2
            for(j=2; j<=63; ++j)
            {
                cpu_idex(CPUInfo, i, j);
                if (0!=CPUInfo[0]
                    || 0!=CPUInfo[1]
                    || 0!=CPUInfo[2]
                    || 0!=CPUInfo[3])
                {
                    RefreshInfo_Put(i, j, CPUInfo);
                }
            }
        }
    }

    // 扩展功能.
    cpuid(CPUInfo, 0x80000000U);
    RefreshInfo_Put(0x80000000U, 0, CPUInfo);
    nmax = CPUInfo[0];	// CPUID(0x80000000).EAX[31:0]=LFuncExt
    _LFuncExt = nmax;
    if (nmax!=0)
    {
        for(i=0x80000001U; i<=nmax; ++i)
        {
            cpu_idex(CPUInfo, i, 0);
            RefreshInfo_Put(i, 0, CPUInfo);
            // fidsub
            if (0x8000001DU==i)	// Cache Properties (Function 8000001Dh)
            {
                j=1;
                while(true)
                {
                    cpu_idex(CPUInfo, i, j);
                    if (0==(CPUInfo[0]&0x1F))	break;	// EAX[4:0]=Cache_Type
                    RefreshInfo_Put(i, j, CPUInfo);
                    // next
                    ++j;
                }
            }
        }
    }

}

// 刷新属性.
void CCPUID::RefreshProperty()
{
    uint32_t dwBuf[4];

    // Vendor
    GetData(dwBuf, 0);	// Function 0: Vendor-ID and Largest Standard Function
    uint32_t* pVendor = (uint32_t *)_Vendor;	// 避免GCC的 -Wstrict-aliasing 警告.
    pVendor[0] = dwBuf[1];	// ebx: 前四个字符.
    pVendor[1] = dwBuf[3];	// edx: 中间四个字符.
    pVendor[2] = dwBuf[2];	// ecx: 最后四个字符.
    _Vendor[12] = '\0';

    // Brand
    _Brand[0] = '\0';
    if (_LFuncExt >= 0x80000004)
    {
        // Function 80000002h,80000003h,80000004h: Processor Brand String
        GetData((uint32_t *)&_Brand[0], 0x80000002);	// 前16个字符.
        GetData((uint32_t *)&_Brand[16], 0x80000003);	// 中间16个字符.
        GetData((uint32_t *)&_Brand[32], 0x80000004);	// 最后16个字符.
        _Brand[48] = '\0';
    }
    _BrandTrim = &_Brand[0];
    while('\0'!=*_BrandTrim && ' '==*_BrandTrim) ++_BrandTrim;	// 去除首都空格.

    // SIMD
    _mmx = simd_mmx(&_hwmmx);
    _sse = simd_sse_level(&_hwsse);
    _avx = simd_avx_level(&_hwavx);

}

// 刷新所有.
void CCPUID::RefreshAll()
{
    RefreshInfo();
    RefreshProperty();
}

// 取得信息.
//
// return: 成功时返回LPCPUIDINFO. 如果失败, 便返回NULL.
// InfoType: 功能号. 即CPUID指令的eax参数.
// ECXValue: 子功能号. 即CPUID指令的ecx参数.
LPCCPUIDINFO CCPUID::GetInfo(uint32_t InfoType, uint32_t ECXValue) const
{
    // 顺序搜索.
    for(int i=0; i<InfoCount(); ++i)
    {
        const CPUIDINFO& v = Info[i];
        if (InfoType==v.fid && ECXValue==v.fidsub)
        {
            return &v;
        }
    }
    return NULL;
}

// 取得数据.
//
// CPUInfo: 成功时返回4个DWORD. 如果失败, 便返回全0.
// InfoType: 功能号. 即CPUID指令的eax参数.
// ECXValue: 子功能号. 即CPUID指令的ecx参数.
void CCPUID::GetData(uint32_t CPUInfo[4], uint32_t InfoType, uint32_t ECXValue) const
{
    LPCCPUIDINFO p = GetInfo(InfoType, ECXValue);
    if (NULL==p)
    {
        CPUInfo[0] = 0;
        CPUInfo[1] = 0;
        CPUInfo[2] = 0;
        CPUInfo[3] = 0;
        return;
    }
    CPUInfo[0] = p->dw[0];
    CPUInfo[1] = p->dw[1];
    CPUInfo[2] = p->dw[2];
    CPUInfo[3] = p->dw[3];
}

// 取得CPUID字段
uint32_t CCPUID::GetField(CPUIDFIELD cpuf) const
{
    LPCCPUIDINFO p = GetInfo(CPUIDFIELD_FID(cpuf), CPUIDFIELD_FIDSUB(cpuf));
    if (NULL==p)	return 0;
    return cpuid_field_buff(p->dw, cpuf);
}

int	CCPUID::simd_mmx(int* phwmmx) const
{
    int	rt = 0;	// result
    #ifdef CCPUID_X86
        const uint32_t	BIT_D_MMX = 0x00800000;	// bit 23
        uint32_t dwBuf[4];

        // check processor support
        GetData(dwBuf, 1);	// Function 1: Feature Information
        if ( dwBuf[3] & BIT_D_MMX )	rt=1;
        if (NULL!=phwmmx)	*phwmmx=rt;

        // check OS support
        rt = -1;	// 需要时才检查, 见mmx().
    #else	// #ifdef CCPUID_X86
        if (NULL!=phwmmx)	*phwmmx=rt;
    #endif	// #ifdef CCPUID_X86
    return rt;
}

int	CCPUID::simd_sse_level(int* phwsse) const
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
        GetData(dwBuf, 1);	// Function 1: Feature Information
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
        rt = -1;	// 需要时才检查, 见sse().
    #else	// #ifdef CCPUID_X86
        if (NULL!=phwsse)	*phwsse=rt;
    #endif	// #ifdef CCPUID_X86
    return rt;
}

int	CCPUID::simd_avx_level(int* phwavx) const
{
    int	rt = SIMD_AVX_NONE;	// result
    #ifdef CCPUID_X86
        // check processor support
        if (0!=GetField(CPUF_AVX))
        {
            rt = SIMD_AVX_1;
            if (0!=GetField(CPUF_AVX2))
            {
                rt = SIMD_AVX_2;
            }
        }
        if (NULL!=phwavx)	*phwavx=rt;

        // check OS support
        if ( rt>0 )
        {
            if (0!=GetField(CPUF_OSXSAVE))	// XGETBV enabled for application use.
            {
                uint32_t n = GetField(CPUF_XFeatureSupportedMaskLo);	// XCR0: XFeatureSupportedMask register.
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

int CCPUID::mmx() const
{
    if (_mmx<0)
    {
        // 发现未检查, 进行检查.
        _mmx = _hwmmx;
        if (_mmx>0)
        {
            if (!simd_try_mmx()) _mmx=0;
        }
    }
    return _mmx;
}

int CCPUID::sse() const
{
    if (_sse<0)
    {
        // 发现未检查, 进行检查.
        _sse = _hwsse;
        if (_sse>0)
        {
            if (!simd_try_sse()) _sse=SIMD_SSE_NONE;
        }
    }
    return _sse;
}

// 打印CPUID字段_某项.
void prtCcpuid_Item(uint32_t fid, uint32_t fidsub, const uint32_t CPUInfo[4])
{
    static const char* RegName[4] = { "EAX", "EBX", "ECX", "EDX" };
    uint32_t mask = CPUIDFIELD_MASK_FID | CPUIDFIELD_MASK_FIDSUB;
    uint32_t cur =  CPUIDFIELD_MAKE(fid, fidsub, 0, 0, 1) & mask;
    int i;
    for(i=0; i<CCPUID::CPUFDescLen; ++i)
    {
        const CPUIDFIELDDESC& v = CCPUID::CPUFDesc[i];
        if ((v.cpuf&mask)==cur)
        {
            CPUIDFIELD f = v.cpuf;
            uint32_t bits = CPUIDFIELD_LEN(f);
            uint32_t pos = CPUIDFIELD_POS(f);
            uint32_t reg = CPUIDFIELD_REG(f);
            uint32_t n = cpuid_field_buff(CPUInfo, f);	//UINT32 n = __GETBITS32(CPUInfo[reg], pos, bits);
            if (bits>1)
            {
                qDebug("\t%s[%2d:%2d]", RegName[reg], pos+bits-1, pos);
            }
            else
            {
                qDebug("\t%s[   %2d]", RegName[reg], pos);
            }
            qDebug("=%s:\t0x%X\t(%u)", v.szName, n, n);
            if (true)
            {
                qDebug("\t// %s", v.szDesc);
            }
        }
    }
}

// 打印CPUID字段.
void prtCcpuid(const CCPUID& ccid)
{
    int i;
    for(i=0; i<ccid.InfoCount(); ++i)
    {
        const CPUIDINFO& v = ccid.Info[i];
        qDebug("0x%.8X[%d]:\t%.8X\t%.8X\t%.8X\t%.8X\n", v.fid, v.fidsub, v.dw[0], v.dw[1], v.dw[2], v.dw[3]);
        // 检查子功能号. 如果是规范的子功能号，便故意设为0，根据子功能号0的字段来解析各个子功能号的信息。
        uint32_t fidsub = v.fidsub;
        switch(v.fid)
        {
        case 0x4: fidsub=0;
        case 0xB: fidsub=0;
        case 0x8000001D: fidsub=0;
        }
        // item
        prtCcpuid_Item(v.fid, fidsub, v.dw);
        // otheritem
        if (0==v.fid)	// Vendor-ID (Function 02h)
        {
            qDebug("\tVendor:\t%s\n", ccid.Vendor());
        }
        else if (0x80000004==v.fid)	// Processor Brand String (Function 80000002h,80000003h,80000004h)
        {
            qDebug("\tBrand:\t%s\n", ccid.Brand());
        }
        else if (0x2==v.fid)	// Cache Descriptors (Function 02h)
        {
            for(int j=0; j<=3; ++j)
            {
                uint32_t n = v.dw[j];
                if (n>0)	// 最高位为0，且不是全0
                {
                    for(int k=0; k<=3; ++k)
                    {
                        if (j>0 || k>0)	// EAX的低8位不是缓存信息
                        {
                            int by = n & 0x00FF;
                            if (by>0)
                            {
                                qDebug("\t0x%.2X:\t%s\n", by, CCPUID::CacheDesc[by]);
                            }
                        }
                        n >>= 8;
                    }
                }
            }
        }
    }
}

void  CCPUID::test_main_()
{
    int i;

    CCPUID& ccid = CCPUID::cur();

    // base info
    qDebug("CCPUID.Vendor:\t%s\n", ccid.Vendor());
    qDebug("CCPUID.BrandTrim:\t%s\n", ccid.BrandTrim());
    qDebug("CCPUID.InfoCount:\t%d\n", ccid.InfoCount());
    qDebug("CCPUID.LFuncStd:\t%.8Xh\n", ccid.LFuncStd());
    qDebug("CCPUID.LFuncExt:\t%.8Xh\n", ccid.LFuncExt());

    // simd info
    qDebug("CCPUID.MMX:\t%d\t// hw: %d\n", ccid.mmx(), ccid.hwmmx());
    //test2();	// 测试多文件链接ccpuid.
    qDebug("CCPUID.SSE:\t%d\t// hw: %d\n", ccid.sse(), ccid.hwsse());
    for(i=1; i<(int)(sizeof(CCPUID::SseNames)/sizeof(CCPUID::SseNames[0])); ++i)
    {
        if (ccid.hwsse()>=i)	qDebug("\t%s\n", CCPUID::SseNames[i]);
    }
    qDebug("SSE4A:\t%d\n", ccid.GetField(CPUF_SSE4A));
    qDebug("AES:\t%d\n", ccid.GetField(CPUF_AES));
    qDebug("PCLMULQDQ:\t%d\n", ccid.GetField(CPUF_PCLMULQDQ));
    qDebug("CCPUID.AVX:\t%d\t// hw: %d\n", ccid.avx(), ccid.hwavx());
    for(i=1; i<(int)(sizeof(CCPUID::AvxNames)/sizeof(CCPUID::AvxNames[0])); ++i)
    {
        if (ccid.hwavx()>=i)	qDebug("\t%s\n", CCPUID::AvxNames[i]);
    }
    qDebug("F16C:\t%d\n", ccid.GetField(CPUF_F16C));
    qDebug("FMA:\t%d\n", ccid.GetField(CPUF_FMA));
    qDebug("FMA4:\t%d\n", ccid.GetField(CPUF_FMA4));
    qDebug("XOP:\t%d\n", ccid.GetField(CPUF_XOP));

    // field info
    qDebug("== fields ==\n");
    prtCcpuid(ccid);
}
QString CCPUID::serial_number()
{
    static char buff[64] = {0};

    uint eax = 0, edx = 0, ecx = 0;

    asm volatile ("cpuid"
                 : "=a"(eax), "=d"(edx)
                 : "a"(0x01)
                 );
    qDebug() << "serial_number";
    if (edx & (1 << 17))
    {
       asm volatile ("cpuid"
                     : "=c"(ecx), "=d"(edx)
                     : "a"(0x03)
                     );

        sprintf (buff, "0x%04x-0x%04x-0x%04x-0x%04x-0x%04x-0x%04x",
                 eax >> 16, (eax << 16) >> 16, edx >> 16, (edx << 16) >> 16,
                 ecx >> 16, (ecx << 16) >> 16);
       return QString(buff);

    }

    return QString("Serial Number not supported.");
}
