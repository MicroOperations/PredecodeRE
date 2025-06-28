#ifndef _UTILS_H_
#define _UTILS_H_

#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/version.h>
#include <linux/smp.h>
#include <linux/sysfs.h>
#include <linux/kprobes.h>
#include <linux/rwlock.h>
#include <linux/preempt.h>
#include <linux/pgtable.h>
#include <linux/pg.h>
#include <linux/mm.h>
#include <asm/pgtable.h>

#define meow(level, fmt, ...) \
    printk(level "PredecodeRE (%s): " fmt "\n", __FUNCTION__, ##__VA_ARGS__)

#endif