#include "include/ioctl.h"
#include "include/core.h"

struct file_operations predecode_re_ops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = ioctl_handler,
};

#define IOCTL_CHECK_MIS _IO(69, 0)
#define IOCTL_CORE _IOR(69, 1, unsigned int)
#define IOCTL_PMC _IOR(69, 2, unsigned int)

extern struct predecode_re rawr;

long ioctl_handler(struct file *file, unsigned int cmd, unsigned long arg)
{
    switch (cmd) {
       
        case IOCTL_CHECK_MIS:

            if (rawr.ioctl.core != smp_processor_id())
                return -EINVAL;

            u32 pmc_no = rawr.params.pmc_no;
            u32 pmc_msr = rawr.ioctl.pmc_msr;

            u64 count = 0;
            for (u32 i = 0; i < PRED_NO_BLOCKS; i++) {

                char *cacheline = rawr.ioctl.cachemem + (i * PRED_BLOCK_SIZE);
                zero_enabled_pmc(pmc_msr, pmc_no);

                __asm__ __volatile__ (
                    "movl %[pmc_no], %%edi;"
                    "call *%[func];"
                    :"=a"(count)
                    :[func]"r"(cacheline), 
                     [pmc_no]"r"(pmc_no)
                    :"%rcx", "%rdx", "%rsi", "%rdi", "%r8");

                if (count > 0)
                    count++;
            }

            meow(KERN_DEBUG, "count: %llu", count);
            break;

        case IOCTL_CORE:
            if (copy_to_user((void *)arg, &rawr.ioctl.core, sizeof(u32)) > 0)
                return -EFAULT;

            break;

        case IOCTL_PMC:
            if (copy_to_user((void *)arg, &rawr.params.pmc_no, sizeof(u32)) > 0)
                return -EFAULT;
                
            break;

        default:
            return -EINVAL;
    }
    return 0;
}