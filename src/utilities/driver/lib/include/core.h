#ifndef _CORE_H_
#define _CORE_H_

#include "utils.h"
#include "arch.h"

struct predecode_re 
{
    struct 
    {
        int *cpu;
    } params;

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

void do_analysis(struct predecode_re *rawr);

#endif