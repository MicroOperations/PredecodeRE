#ifndef _CORE_H_
#define _CORE_H_

#include "utils.h"
#include "arch.h"

typedef unsigned long (*kallsyms_ln_t)(const char *name);
typedef int (*set_memory_x_t)(unsigned long addr, int numpages);

#define PC_CACHE_SIZE 65536
#define PC_CACHE_PAGES (PC_CACHE_SIZE/PAGE_SIZE)
#define PC_NO_64B_BLOCKS (PC_CACHE_SIZE/64)

struct predecode_re 
{
    struct 
    {
        int *cpu;
    } params;

    struct 
    {
        kallsyms_ln_t kallsyms_ln;
        set_memory_x_t set_mem_x;
    } func_ptrs;

    struct
    {
        int ret;
    } analysis;

    struct
    {
        struct kobj_attribute *attr;
        struct kobject *kobj_ref;
        bool sysfs_setup;
    } sysfs;

    struct mutex lock;
};

extern u8 benchmark_routine[];

void do_analysis(struct predecode_re *rawr);

#endif