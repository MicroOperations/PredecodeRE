#include <linux/module.h>
#include <linux/moduleparam.h>

#include "lib/include/sys.h"

MODULE_AUTHOR("MicroOperations");
MODULE_DESCRIPTION("Driver for our predecode cache reversing");
MODULE_VERSION("1.0.0");
MODULE_LICENSE("GPL");

static int cpu = -1;
module_param(cpu, int, 0);
MODULE_PARM_DESC(cpu, "Specific cpu to test on (< 0 or no arg for default)");

struct predecode_re rawr = {

    .params = {
        .cpu = &cpu,
    },

    .analysis = {},

    .sysfs = {
        .attr = &predecode_re_attr,
        .kobj_ref = NULL,
        .sysfs_setup = false
    },

    .lock = __MUTEX_INITIALIZER(rawr.lock),
};

#define SYSFS_DIR_NAME "predecode_re"

static int __init driver_entry(void)
{
    meow(KERN_DEBUG, "driver loaded");

    if (*rawr.params.cpu < 0) {
        *rawr.params.cpu = smp_processor_id();
        do_analysis(&rawr);

    } else {
        int errcode = smp_call_function_single(*rawr.params.cpu, 
                      (smp_call_func_t)do_analysis, &rawr, 1);

        if (errcode < 0) {
            meow(KERN_ERR, "failed to do analysis on the passed core");
            rawr.analysis.ret = errcode;
        }
    }

    if (rawr.analysis.ret < 0) {
        meow(KERN_ERR, "analysis failed");
        return rawr.analysis.ret;
    }

    meow(KERN_DEBUG, "analysis successful");
 
    struct kobject *ref = kobject_create_and_add(SYSFS_DIR_NAME, kernel_kobj);
    if (!ref) {
        meow(KERN_ERR, "failed to create and add kobject");
        return -EFAULT;
    }

    if (sysfs_create_file(ref, &rawr.sysfs.attr->attr)) {
        meow(KERN_ERR, "failed to create sysfs file");
        kobject_put(ref);
        return -EFAULT;
    }

    rawr.sysfs.kobj_ref = ref;
    rawr.sysfs.sysfs_setup = true;
   
    meow(KERN_DEBUG, "driver setup successfully");
    return 0;
}

static void __exit driver_exit(void)
{
    mutex_lock(&rawr.lock);

    if (rawr.sysfs.sysfs_setup)
        sysfs_remove_file(rawr.sysfs.kobj_ref, &rawr.sysfs.attr->attr);

    if (rawr.sysfs.kobj_ref) 
        kobject_put(rawr.sysfs.kobj_ref);
    
    mutex_unlock(&rawr.lock);

    meow(KERN_DEBUG, "driver unloaded");
}

module_init(driver_entry);
module_exit(driver_exit);