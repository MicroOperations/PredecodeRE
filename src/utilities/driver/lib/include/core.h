#ifndef _CORE_H_
#define _CORE_H_

#include "utils.h"
#include "arch.h"

typedef unsigned long (*kallsyms_ln_t)(const char *name);
typedef int (*set_memory_x_t)(unsigned long addr, int numpages);

#define PRED_CACHE_SIZE 65536 // the predecode cache is 64kb so yeyeyeyeye
#define PRED_BLOCK_SIZE 64 // we finna go up dis bihhh in 64b blocks

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
    } func_ptrs;

    struct
    {      
        struct 
        {   
            u64 first_iter;
            u64 second_iter;

            u64 avg1;
            u64 avg2;
            u64 total_avg;
        } base;

        u64 ways_per_set;
        u64 cacheline_size;
    } analysis;

    struct
    {
        struct kobj_attribute *attr;
        struct kobject *kobj_ref;
        bool sysfs_setup;
    } sysfs;

    struct mutex lock;
};

struct reverse_pred_cache
{
    struct predecode_re *rawr;

    char *predecode_cache;
    size_t predecode_cache_size;

    size_t block_size;

    u32 pmc_msr;
    u32 pmc_no;
};

extern u8 benchmark_routine[];

int __do_reverse_pred_cache(struct reverse_pred_cache *arg);
int __reverse_pred_cache(struct predecode_re *rawr, u32 pmc_msr, u32 pmc_no);

int __do_analysis(struct predecode_re *rawr);
int __analysis(struct predecode_re *rawr);

#endif