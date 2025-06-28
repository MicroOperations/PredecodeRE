#include "include/core.h"

/* 

    * formatted this in intel syntax for you 
      skid fucks 

nop
nop
nop
xor eax, eax
xor esi, esi
mov r8, cr3
xor ecx, ecx
mov cr3, r8
rdpmc
mov esi, eax
mov edi, edx
xor eax, eax
add ax, 4
shr ax, 1
sub ax, 2
movzx ecx, ax
rdpmc
mov cr3, r8
shl rdi, 32
shl rdx, 32
or rsi, rdi
or rax, rdx
sub rax, rsi
ret

rsi = count 1 
rax = count 2
*/
u8 benchmark_routine[] = 
{ 
    0x90, 0x90, 0x90, 0x90, 0x31, 0xC0, 0x31, 0xF6, 0x41, 0x0F, 0x20, 
    0xD8, 0x31, 0xC9, 0x41, 0x0F, 0x22, 0xD8, 0x0F, 0x33, 0x89, 0xC6, 
    0x89, 0xD7, 0x31, 0xC0, 0x66, 0x83, 0xC0, 0x04, 0x66, 0xD1, 0xE8, 
    0x66, 0x83, 0xE8, 0x02, 0x0F, 0xB7, 0xC8, 0x0F, 0x33, 0x41, 0x0F, 
    0x22, 0xD8, 0x48, 0xC1, 0xE7, 0x20, 0x48, 0xC1, 0xE2, 0x20, 0x48, 
    0x09, 0xFE, 0x48, 0x09, 0xD0, 0x48, 0x29, 0xF0, 0xC3 
};

void do_analysis(struct predecode_re *rawr)
{
    if (!rawr)
        return;

    /* get exported symbols we need */
    struct kprobe kp = {.symbol_name = "kallsyms_lookup_name"};
    register_kprobe(&kp);
    kallsyms_ln_t kallsyms_ln = (kallsyms_ln_t)kp.addr;
    unregister_kprobe(&kp);

    if (!kallsyms_ln) {
        meow(KERN_ERR, "couldnt get kallsyms fuck");
        rawr->analysis.ret = -EFAULT;
        return;
    }

    set_memory_x_t set_mem_x = (set_memory_x_t)kallsyms_ln("set_memory_x");
    if (!set_mem_x) {
        meow(KERN_ERR, "couldnt get set memory x fuck");
        rawr->analysis.ret = -EFAULT;
        return;
    }

    rawr->func_ptrs.kallsyms_ln = kallsyms_ln;
    rawr->func_ptrs.set_mem_x = set_mem_x;

    /* map physically contingous memory, this finna be large enough 
       to fill the 64kb predecode cache, then mark it as executeable */
    char *predecode_cache = kzalloc(PC_CACHE_SIZE, GFP_KERNEL);
    if (!predecode_cache) {
        meow(KERN_ERR, "couldnt alloc predecode cache mem");
        rawr->analysis.ret = -ENOMEM;
        return;
    }

    set_mem_x((unsigned long)predecode_cache, PC_CACHE_PAGES);

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
    evt.fields.umask = ICACHE_MISSES_UMASK;
    evt.fields.evtsel = ICACHE_MISSES_EVTSEL;
    evt.fields.enable_pmc = true;
    __wrmsrl(IA32_PERFEVTSEL0, evt.val);

    /* enable da bitch */
    enable_pmc(0);

    /* get to base benchmarks */
    u64 totals[128] = {0};
    u64 first_iter = 0;
    u64 second_iter = 0;
    for (u32 i = 0; i < ARRAY_SIZE(totals); i++) {

        /* zero out the pmc to minimise chance of overflow */
        zero_enabled_pmc(pmc0_msr, 0);

        u64 count = 0;
        __asm__ __volatile__(

            ".align 64;"

            /* setup r8 with cr3 since reads from cr3
               arent serialised we will have to use
               writes */
            "movq %%cr3, %%r8;"

            /* count pmc0 */
            "xorl %%ecx, %%ecx;"

            /* 'serialise' just this code block */
            "wbinvd;"
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
            "subq %%rsi, %%rax;"
            : "=a"(count)
            :
            : "%rcx", "%r8", "%rdx", "%rdi");

        totals[i] = count;

        if (i == 0) 
            first_iter = totals[i];
        else if (i == 1)
            second_iter = totals[i];
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
    
    meow(KERN_DEBUG, "basic timing analysis");
    meow(KERN_DEBUG, "first iter: %llu: second iter: %llu", 
         first_iter, second_iter);
    meow(KERN_DEBUG, "avg1: %llu avg2: %llu total avg: %llu",
         avg1, avg2, total_avg);

    u64 avg_block_times[PC_NO_64B_BLOCKS] = {0};
    for (u32 i = 0; i < PC_NO_64B_BLOCKS; i++) {
        __asm__ __volatile__ (
            "rep movsq;"
            :
            :"S"(benchmark_routine), 
             "D"(predecode_cache + (i * 64)), 
             "c"(sizeof(benchmark_routine)/8)
            :"memory"
        );
    }

    /* disable and zero the pmc */
    disable_pmc(0);
    __wrmsrl(pmc0_msr, 0);
    __wrmsrl(IA32_PERFEVTSEL0, 0);

    /* free the predecode cache mem */
    kfree(predecode_cache);

    /* return */
    rawr->analysis.ret = 0;
}