#ifndef _CORE_H_
#define _CORE_H_

#include "utils.h"
#include "arch.h"

typedef unsigned long (*kallsyms_ln_t)(const char *name);
typedef int (*set_memory_x_t)(unsigned long addr, int numpages);
typedef int (*set_memory_uc_t)(unsigned long addr, int numpages);
typedef void (*flush_tlb_kernel_range_t)(unsigned long start, unsigned long end);

#define PRED_CACHE_SIZE 65536 // the predecode cache is 64kb so yeyeyeyeye
#define PRED_BLOCK_SIZE 64 // we finna go up dis bihhh in 64b blocks
#define PRED_NO_BLOCKS (PRED_CACHE_SIZE/PRED_BLOCK_SIZE)

struct pmc_event
{
    u32 evtsel;
    u32 umask;
    char *event_name;
};

struct predecode_re 
{
    struct 
    {
        u32 pmc_no;
        struct pmc_event event;
    } params;

    struct 
    {
        kallsyms_ln_t kallsyms_ln;
        set_memory_x_t set_mem_x;
        set_memory_uc_t set_mem_uc;
        flush_tlb_kernel_range_t __flush_tlb_kernel_range;
    } func_ptrs;

    struct
    {      
        struct 
        {   
            u64 avg1;
            u64 avg2;
            u64 total_avg;
        } base;

        u64 eviction_count;
    } analysis;
};

struct reverse_pred_cache
{
    struct predecode_re *rawr;

    char *predecode_cache1;
    char *predecode_cache2;

    u32 no_blocks;
    size_t block_size;

    u32 pmc_msr;
    u32 pmc_no;
};

extern u8 benchmark_routine1[];

int __do_reverse_pred_cache(struct reverse_pred_cache *arg);
int __reverse_pred_cache(struct predecode_re *rawr, u32 pmc_msr, u32 pmc_no);

int __do_analysis(struct predecode_re *rawr);
int __analysis(struct predecode_re *rawr);

#endif