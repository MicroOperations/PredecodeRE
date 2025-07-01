#include <linux/module.h>
#include <linux/moduleparam.h>

#include "lib/include/sys.h"

MODULE_AUTHOR("MicroOperations");
MODULE_DESCRIPTION("Driver for our predecode cache reversing");
MODULE_VERSION("1.0.0");
MODULE_LICENSE("GPL");

static u32 pmc_no = 0;
module_param(pmc_no, uint, 0);
MODULE_PARM_DESC(pmc_no, "pmc no (default: 0)");

static u32 pmc_event_no = 0;
module_param(pmc_event_no, uint, 0);
MODULE_PARM_DESC(pmc_event_no,
"pmc event no (default: 0):\n"
"   0: predecode cache mispred\n"
"   1: icache miss\n"
"   2: machine clears\n"
"   3. unhalted core cycles");

/* ur so pawesome u have a fluffy tail and ur 
   paws r so fuzzy ur so meow meow, come here
   and lemme pet u, lets cuddle, ^~_~^ purrrr
   purrrr meow meow meow meow mrrrrooooowwww */

#define SYSFS_DIR_NAME "predecode_re"

struct predecode_re rawr = {

    .params = {
        .pmc_no = 0,
        .event = {},
    },

    .analysis = {},

    .sysfs = {
        .attr = &predecode_re_attr,
        .kobj_ref = NULL,
        .sysfs_setup = false
    },

    .lock = __MUTEX_INITIALIZER(rawr.lock),
};

struct pmc_event pmc_events[] = {
    {PRED_WRONG_EVTSEL, PRED_WRONG_UMASK, "predecode cache mispredictions"},
    {ICACHE_MISSES_EVTSEL, ICACHE_MISSES_UMASK, "icache misses"},
    {MACHINE_CLEARS_EVTSEL, MACHINE_CLEARS_UMASK, "machine clears"},
    {UNHALTED_CYCLES_EVTSEL, UNHALTED_CYCLES_UMASK, "unhalted core cycles"}
};

static int __init driver_entry(void)
{
    meow(KERN_DEBUG, "driver loaded");


    /* validate params */
    if (!is_arch_pmc_no_supported(pmc_no)) {
        meow(KERN_ERR, "invalid pmc no");
        return -EINVAL;   
    }

    if (pmc_event_no >= ARRAY_SIZE(pmc_events)) {
        meow(KERN_ERR, "invalid event option");
        return -EINVAL;
    }

    rawr.params.pmc_no = pmc_no;
    rawr.params.event = pmc_events[pmc_event_no];

    /* do the frickin analysis shit */
    int err = __analysis(&rawr);
    if (err < 0) {
        meow(KERN_ERR, "analysis failed");
        return err;
    }

    meow(KERN_DEBUG, "analysis successful");
 
    /* setup the sysfs driver */
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

    /* driver setup */
    rawr.sysfs.kobj_ref = ref;
    rawr.sysfs.sysfs_setup = true;
   
    meow(KERN_DEBUG, "driver setup successfully");
    return 0;
}

static void __exit driver_exit(void)
{
    /* clean shit up usual biz */

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