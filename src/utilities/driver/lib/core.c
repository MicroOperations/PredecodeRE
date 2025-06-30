#include "include/core.h"

/* 

* formatted this in intel syntax so
  i dont get yelled at by the skids

nop dword ptr [rax+rax*1+0x0]
nop
nop
mov ecx, edi
mov r8, cr3
mov cr3, r8
rdpmc
mov esi, eax
mov edi, edx
mov eax, ecx
add ax, 4
sub ax, 2
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

rax = count

u64 benchmark_routine(u32 pmc_no);

*/
u8 benchmark_routine[] = 
{
     0x0F, 0x1F, 0x44, 0x00, 0x00, 0x90, 0x90, 0x89, 
     0xF9, 0x41, 0x0F, 0x20, 0xD8, 0x41, 0x0F, 0x22, 
     0xD8, 0x0F, 0x33, 0x89, 0xC6, 0x89, 0xD7, 0x89, 
     0xC8, 0x66, 0x83, 0xC0, 0x04, 0x66, 0x83, 0xE8, 
     0x02, 0x66, 0x83, 0xE8, 0x02, 0x0F, 0xB7, 0xC8, 
     0x0F, 0x33, 0x41, 0x0F, 0x22, 0xD8, 0x48, 0xC1, 
     0xE7, 0x20, 0x48, 0xC1, 0xE2, 0x20, 0x48, 0x09, 
     0xFE, 0x48, 0x09, 0xD0, 0x48, 0x29, 0xF0, 0xC3 
};

int __do_reverse_pred_cache(struct reverse_pred_cache *arg)
{
    size_t cache_size = arg->predecode_cache_size;
    size_t block_size = arg->block_size;
    u32 pmc_msr = arg->pmc_msr;
    u32 pmc_no = arg->pmc_no;

    for (u32 i = 0; i < (cache_size/block_size); i++) {

        u64 initial_count = 0;
        zero_enabled_pmc(pmc_msr, pmc_no);
        __asm__ __volatile__ (
            "movl %[pmc_no], %%edi;"
            "call *%[func];"
            :"=a"(initial_count)
            :[func]"r"(arg->predecode_cache + (i * block_size)), 
             [pmc_no]"r"(pmc_no)
            :"%rcx", "%rdx", "%rsi", "%rdi", "%r8");
        
        for (u32 j = 0; j < i; j++) {

            u64 re_count = 0;
            zero_enabled_pmc(pmc_msr, pmc_no);
            __asm__ __volatile__ (
                "movl %[pmc_no], %%edi;"
                "call *%[func];"
                :"=a"(re_count)
                :[func]"r"(arg->predecode_cache + (j * block_size)),
                 [pmc_no]"r"(pmc_no)
                :"%rcx", "%rdx", "%rsi", "%rdi", "%r8");
        }
    }

    return 0;
}

int __reverse_pred_cache(struct predecode_re *rawr, u32 pmc_msr, u32 pmc_no)
{
    /* map physically contingous memory to fill the 64kb predecode
       cache then copy our benchmark routine into each 64b block
       of it, and set the regions pte's to executeable so we 
       can actually like fuckin execute it since linux will 
       set xd */
    char *predecode_cache = kzalloc(PRED_CACHE_SIZE, GFP_KERNEL);
    if (!predecode_cache) {
        meow(KERN_ERR, "couldnt alloc predecode cache mem");
        return -ENOMEM;
    }

    for (u32 i = 0; i < (PRED_CACHE_SIZE/PRED_BLOCK_SIZE); i++) {
        memcpy(predecode_cache + (i * PRED_BLOCK_SIZE), benchmark_routine,
               sizeof(benchmark_routine));
    }

    rawr->func_ptrs.set_mem_x((unsigned long)predecode_cache, 
                              PRED_CACHE_SIZE/PAGE_SIZE);
    
    struct reverse_pred_cache arg = {
        .rawr = rawr,
        .predecode_cache = predecode_cache,
        .predecode_cache_size = PRED_CACHE_SIZE,
        .block_size = PRED_BLOCK_SIZE,
        .pmc_msr = pmc_msr,
        .pmc_no = pmc_no,
    };

    /* reverse engineer the predecode cache on the meow meow core rawrrr */
    int ret = stop_machine((cpu_stop_fn_t)__do_reverse_pred_cache, &arg, 
                           cpumask_of(smp_processor_id()));

    kfree(predecode_cache);
    return ret;
}

int __do_analysis(struct predecode_re *rawr)
{
    /* disable, zero out and setup the pmc */
    u32 pmc_no = rawr->params.pmc_no;
    u32 pmc_msr = (fw_a_pmc_supported(pmc_no) ? 
                   IA32_A_PMC0 : IA32_PMC0) + pmc_no;

    disable_pmc(pmc_no);
    __wrmsrl(pmc_msr, 0);

    ia32_perfevtsel_t evt = {
        .fields.os = true,
        .fields.umask = rawr->params.event.umask,
        .fields.evtsel = rawr->params.event.evtsel,
        .fields.enable_pmc = true,
    };

    __wrmsrl(IA32_PERFEVTSEL0 + pmc_no, evt.val);
    enable_pmc(pmc_no);

    /* get base reads of the event counter */
    u64 totals[128] = {0};
    for (u32 i = 0; i < ARRAY_SIZE(totals); i++) {

        zero_enabled_pmc(pmc_msr, pmc_no);
        __asm__ __volatile__(

            /* setup ecx with the pmc */
            "movl %[pmc_no], %%ecx;"

            /* setup r8 with cr3 since reads from cr3
               arent serialised we will have to use
               writes */
            "movq %%cr3, %%r8;"

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

            /* setup eax with the pmc so we can setup 
               ecx to be dependant on rax */
            "movl %%ecx, %%eax;"

            /* measured instructions (lcp heavy) */
            "addw $4, %%ax;"
            "subw $2, %%ax;"
            "subw $2, %%ax;"

            /* make the rdpmc stall for dependancy on rax */
            "movzx %%ax, %%ecx;"
            "rdpmc;"
            "movq %%r8, %%cr3;"

            /* count1 into rsi & count2 into rax */
            "shlq $32, %%rdi;"
            "shlq $32, %%rdx;"
            "orq %%rdi, %%rsi;"
            "orq %%rdx, %%rax;"

            /* get the difference of count 1 and 2 */
            "subq %%rsi, %%rax;"
            : "=a"(totals[i])
            : [pmc_no]"r"(pmc_no)
            : "%rcx", "%rdx", "%rsi", "%rdi", "%r8");
    }

    /* get averages and store back the results */
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

    rawr->analysis.base.first_iter = totals[0];
    rawr->analysis.base.second_iter = totals[1];
    rawr->analysis.base.avg1 = avg1;
    rawr->analysis.base.avg2 = avg2;
    rawr->analysis.base.total_avg = total_avg;
    
    meow(KERN_DEBUG, "base event counts:");
    meow(KERN_DEBUG, "first iter: %llu: second iter: %llu", 
         totals[0], totals[1]);
    meow(KERN_DEBUG, "avg1: %llu avg2: %llu total avg: %llu",
         avg1, avg2, total_avg);

    /* reverse engineer the predecode cache */
    int ret = __reverse_pred_cache(rawr, pmc_msr, pmc_no);

    /* disable and zero the pmc, were done now */
    disable_pmc(pmc_no);
    __wrmsrl(pmc_msr, 0);
    __wrmsrl(IA32_PERFEVTSEL0, 0);

    return ret;
}

int __analysis(struct predecode_re *rawr)
{       
    /* get exported symbols we need */
    struct kprobe kp = {.symbol_name = "kallsyms_lookup_name"};

    register_kprobe(&kp);
    kallsyms_ln_t kallsyms_ln = (kallsyms_ln_t)kp.addr;
    unregister_kprobe(&kp);
    if (!kallsyms_ln) {
        meow(KERN_ERR, "couldnt get kallsyms fuck");
        return -EFAULT;
    }

    set_memory_x_t set_mem_x = (set_memory_x_t)kallsyms_ln("set_memory_x");
    if (!set_mem_x) {
        meow(KERN_ERR, "couldnt get set memory x fuck");
        return -EFAULT;
    }

    rawr->func_ptrs.kallsyms_ln = kallsyms_ln;
    rawr->func_ptrs.set_mem_x = set_mem_x;
        
    /* start analysis routine */
    return __do_analysis(rawr);
}