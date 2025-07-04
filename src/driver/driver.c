#include <linux/module.h>
#include <linux/moduleparam.h>

#include "lib/include/core.h"
#include "lib/include/ioctl.h"

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

struct predecode_re rawr = {

    .params = {},
    .func_ptrs = {},
    .analysis = {},
    .ioctl = {
        .chardev = NULL,
        .file_ops = &predecode_re_ops,
    },
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
 
    /* setup the ioctl driver */
    char *cachemem = kzalloc(PRED_CACHE_SIZE, GFP_KERNEL);
    if (!cachemem) {
        meow(KERN_ERR, "couldnt alloc cachemem");
        return -ENOMEM;
    }

    for (u32 i = 0; i < PRED_NO_BLOCKS; i++) 
        memcpy(cachemem + (i * PRED_BLOCK_SIZE), benchmark_routine1, 64);

    rawr.func_ptrs.set_mem_x((unsigned long)cachemem, PRED_CACHE_SIZE/PAGE_SIZE);

    rawr.ioctl.pmc_msr = (fw_a_pmc_supported(rawr.params.pmc_no) ? 
                              IA32_A_PMC0 : IA32_PMC0) + rawr.params.pmc_no;

    rawr.ioctl.core = smp_processor_id();

    disable_pmc(rawr.params.pmc_no);
    __wrmsrl(rawr.ioctl.pmc_msr, 0);

    ia32_perfevtsel_t evt = {
        .fields.os = true,
        .fields.usr = true,
        .fields.umask = rawr.params.event.umask,
        .fields.evtsel = rawr.params.event.evtsel,
        .fields.enable_pmc = true,
    };

    __wrmsrl(IA32_PERFEVTSEL0 + rawr.params.pmc_no, evt.val);
    enable_pmc(rawr.params.pmc_no);
    toggle_user_rdpmc(true);

    struct chardev *chardev = alloc_chardev(DEVICE_NAME, rawr.ioctl.file_ops,
                                            CLASS_NAME, NULL, 
                                            &rawr, CHARDEV_NAME);

    if (IS_ERR(chardev)) {
        meow(KERN_ERR, "failed to setup chardev");

        disable_pmc(rawr.params.pmc_no);
        __wrmsrl(rawr.ioctl.pmc_msr, 0);
        __wrmsrl(IA32_PERFEVTSEL0 + rawr.params.pmc_no, 0);
        toggle_user_rdpmc(false);
        
        kfree(cachemem);
        return PTR_ERR(chardev);
    }

    rawr.ioctl.chardev = chardev;
    rawr.ioctl.cachemem = cachemem;
   
    meow(KERN_DEBUG, "driver setup successfully");
    return 0;
}

static void __exit driver_exit(void)
{
    /* clean shit up usual biz */
    free_chardev(rawr.ioctl.chardev);    

    disable_pmc(rawr.params.pmc_no);
    __wrmsrl(rawr.ioctl.pmc_msr, 0);
    __wrmsrl(IA32_PERFEVTSEL0 + pmc_no, 0);
    toggle_user_rdpmc(false);

    kfree(rawr.ioctl.cachemem);

    meow(KERN_DEBUG, "driver unloaded");
}

module_init(driver_entry);
module_exit(driver_exit);