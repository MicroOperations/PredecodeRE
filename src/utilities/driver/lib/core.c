#include "include/core.h"

void do_analysis(struct predecode_re *rawr)
{
    if (!rawr)
        return;

    /* rawdog it and use the first pmc */

    /* get caps */
    u32 pmc0_msr = fw_a_pmc_supported(0) ? IA32_A_PMC0 : IA32_PMC0;

    /* first disable da bitch */
    disable_pmc(0);

    /* zero da bitch */
    __wrmsrl(pmc0_msr, 0);

    /* setup da bitch */
    ia32_perfevtsel_t evt = {0};
    evt.fields.os = true;
    evt.fields.umask = UNHALTED_CORE_CYCLES_UMASK;
    evt.fields.evtsel = UNHALTED_CORE_CYCLES_EVTSEL;
    evt.fields.enable_pmc = true;
    __wrmsrl(IA32_PERFEVTSEL0, evt.val);

    /* enable da bitch */
    enable_pmc(0);

    /* get to benchmarking to infer how evictions are caused */

    u64 totals[128] = {0};
    u64 first_iter = 0;
    /* first determine average cycle count */
    for (u32 i = 0; i < ARRAY_SIZE(totals); i++) {

        u64 count1 = 0;
        u64 count2 = 0;
        __asm__ __volatile__(

            /* setup r8 with cr3 since reads from cr3
               arent serialised we will have to use
               writes */
            "movq %%cr3, %%r8;"

            /* count pmc0 */
            "xorl %%ecx, %%ecx;"

            "wbinvd;"
            
            /* 'serialise' just this code block */
            "movq %%r8, %%cr3;"
            "rdpmc;"

            /* time constraints here, want to get
               this block executed asap so just
               save the low and high val for later,
               will be quick due to move elimination
               anyway */

            "movl %%eax, %%esi;"
            "movl %%edx, %%edi;"

            /* zero rax dafuq out */
            "xorl %%eax, %%eax;"

            /* measured instructions (lcp heavy) */
            "addw $4, %%ax;"
            "shrw $1, %%ax;"
            "subw $2, %%ax;"

            /* make the rdpmc stall for dependancy on ax */
            "movzx %%ax, %%ecx;"
            "rdpmc;"
            "movq %%r8, %%cr3;"

            /* count1 into rsi & count2 into rax */
            "shlq $32, %%rdi;"
            "shlq $32, %%rdx;"

            "orq %%rdi, %%rsi;"
            "orq %%rdx, %%rax;"
            : "=S"(count1), "=a"(count2)
            :
            : "%rcx", "%r8", "%rdx", "%rdi");

        totals[i] = count2 - count1;
        if (i == 0) 
            first_iter = totals[i];

        zero_enabled_pmc(pmc0_msr, 0);
    }

    /* get average */
    u64 avg1 = 0;
    u64 avg2 = 0;
    u64 total_avg = 0;

    for (u32 i = 0; i < ARRAY_SIZE(totals)/2; i++) {
        avg1 += totals[i];
        total_avg += totals[i];
    }

    for (u32 i = ARRAY_SIZE(totals)/2; i < ARRAY_SIZE(totals); i++) {
        avg2 += totals[i];
        total_avg += totals[i];
    }
    
    avg1 /= ARRAY_SIZE(totals)/2;
    avg2 /= ARRAY_SIZE(totals)/2;
    total_avg /= ARRAY_SIZE(totals);
    
    meow(KERN_DEBUG, "first: %llu: avg1: %llu avg2: %llu total avg: %llu", 
         first_iter, avg1, avg2, total_avg);

    /* disable and zero the pmc */
    disable_pmc(0);
    __wrmsrl(pmc0_msr, 0);
    __wrmsrl(IA32_PERFEVTSEL0, 0);

    /* return */
    rawr->analysis.ret = 0;
}