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
#include <linux/vmalloc.h>
#include <linux/gfp.h>
#include <linux/stop_machine.h>
#include <linux/ioctl.h>
#include <linux/device.h>
#include <linux/cdev.h>

struct chardev
{
    dev_t dev;
    struct cdev cdev_node;
    struct class *device_class;
    struct device *device_node;
};

/* meow meow mrrrp mrrrowww meow meow mrrrrooowwww ^~_~^ */
#define meow(level, fmt, ...) \
    printk(level "PredecodeRE (%s): " fmt "\n", __FUNCTION__, ##__VA_ARGS__)

struct chardev *alloc_chardev(char *device_name, struct file_operations *fops,
                              char *class_name, struct device *parent,
                              void *drvdata, char *chardev_name);

void free_chardev(struct chardev *chardev);

#endif