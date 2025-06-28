#include "include/arch.h"

inline u64 __rdmsrl(u32 msr)
{
    u32 low = 0;
    u32 high = 0;

    __asm__ __volatile__ ("rdmsr;":"=a"(low), "=d"(high):"c"(msr));

    return ((u64)high << 32) | low;
}

inline void __wrmsrl(u32 msr, u64 val)
{
    __asm__ __volatile__ (
        "movl %%esi, %%eax;"
        "shrq $32, %%rsi;"
        "movl %%esi, %%edx;"
        "wrmsr;"
        :
        :"c"(msr), "S"(val)
    );
}

inline void __prefetcht0(void *addr)
{
    __asm__ __volatile__ ("prefetcht0 (%0)"::"r"(addr));
}

inline void __prefetcht1(void *addr)
{
    __asm__ __volatile__ ("prefetcht1 (%0)"::"r"(addr));
}

inline void __prefetcht2(void *addr)
{
    __asm__ __volatile__ ("prefetcht2 (%0)"::"r"(addr));
}

inline void __prefetchnta(void *addr)
{
    __asm__ __volatile__ ("prefetchnta (%0)"::"r"(addr));
}

inline void __prefetchw(void *addr)
{
    __asm__ __volatile__ ("prefetchw (%0)"::"r"(addr));
}

inline bool is_cpu_intel(void)
{
    u32 regs[4] = {0};
    __cpuid(&regs[0], &regs[1], &regs[2], &regs[3]);
    return regs[1] == GENUINE_INTEL_EBX && 
           regs[3] == GENUINE_INTEL_EDX &&
           regs[2] == GENUINE_INTEL_ECX;
}

inline bool is_arch_lbr_supported(void)
{
    u32 regs[4] = {CPUID_EXTENDED_FEATURES, 0, 0, 0};
    __cpuid(&regs[0], &regs[1], &regs[2], &regs[3]);
    return ((regs[3] >> 19) & 1) != 0;
}

inline bool is_arch_perfmon_ext_supported(void)
{
    u32 regs[4] = {CPUID_EXTENDED_FEATURES, 0, 1, 0};
    __cpuid(&regs[0], &regs[1], &regs[2], &regs[3]);
    return ((regs[0] >> 8) & 1) != 0;
}

inline bool is_invd_prevention_supported(void)
{
    u32 regs[4] = {CPUID_EXTENDED_FEATURES, 0, 1, 0};
    __cpuid(&regs[0], &regs[1], &regs[2], &regs[3]);
    return ((regs[0] >> 30) & 1) != 0;
}

inline bool is_cldemote_supported(void)
{
    u32 regs[4] = {CPUID_EXTENDED_FEATURES, 0, 0, 0};
    __cpuid(&regs[0], &regs[1], &regs[2], &regs[3]);
    return ((regs[2] >> 25) & 1) != 0; 
}

inline void disable_perf_metrics(void)
{
    ia32_perf_global_ctrl_t ctrl = {.val = __rdmsrl(IA32_PERF_GLOBAL_CTRL)};
    ctrl.fields.enable_perf_metrics = false;
    __wrmsrl(IA32_PERF_GLOBAL_CTRL, ctrl.val);
}

inline bool fw_a_pmc_supported(u32 pmc_no)
{
    ia32_perf_capabilities_t caps = {.val = __rdmsrl(IA32_PERF_CAPABILITIES)};
    return ((caps.fields.fw_ia32_a_pmcx >> pmc_no) & 1) != 0;
}

inline void enable_pmc(u32 pmc_no)
{
    ia32_perf_global_ctrl_t ctrl = {.val = __rdmsrl(IA32_PERF_GLOBAL_CTRL)};
    ctrl.fields.enable_pmcn |= (1 << pmc_no);
    __wrmsrl(IA32_PERF_GLOBAL_CTRL, ctrl.val);
}

inline void disable_pmc(u32 pmc_no)
{
    ia32_perf_global_ctrl_t ctrl = {.val = __rdmsrl(IA32_PERF_GLOBAL_CTRL)};
    ctrl.fields.enable_pmcn &= ~(1 << pmc_no);
    __wrmsrl(IA32_PERF_GLOBAL_CTRL, ctrl.val);
}

inline void zero_enabled_pmc(u32 pmc_msr, u32 pmc_no)
{
    disable_pmc(pmc_no);
    __wrmsrl(pmc_msr, 0);
    enable_pmc(pmc_no);
}