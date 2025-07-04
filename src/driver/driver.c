#include <linux/module.h>
#include <linux/moduleparam.h>

#include "lib/include/core.h"

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

#define DEVICE_NAME "predecode_re"
#define CLASS_NAME "predecode_re_class"
#define CHARDEV_NAME "predecode_re"

struct predecode_re rawr = {};

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
    int ret = __analysis(&rawr);
    if (ret < 0) 
        meow(KERN_ERR, "analysis failed");
    else
        meow(KERN_DEBUG, "analysis successful");

    return ret;
}

static void __exit driver_exit(void)
{
    meow(KERN_DEBUG, "driver unloaded");
}

module_init(driver_entry);
module_exit(driver_exit);