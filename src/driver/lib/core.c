#include "include/core.h"

int __do_reverse_pred_cache(struct reverse_pred_cache *arg)
{
    /* we pass shit via struct coz cant alloc mem here 
       or crash bc calling stop_machine */
    char *cache = arg->predecode_cache;
    u32 pmc_msr = arg->pmc_msr;
    u32 pmc_no = arg->pmc_no;

    u64 eviction_count = 0;

    /* re whatever we need to re however we need */

    u64 initial_count = 0;
    zero_enabled_pmc(pmc_msr, pmc_no);
    __asm__ __volatile__ (
        "call *%[func];"
        "movl %[pmc_no], %%ecx;"
        "rdpmc;"
        "shlq $32, %%rdx;"
        "orq %%rdx, %%rax;"
        :"=a"(initial_count)
        :[func]"r"(cache), [pmc_no]"r"(pmc_no)
        :"%rdx", "%ecx"
    );

    cache[PRED_CACHE_SIZE - 1] = 0xc3;

    zero_enabled_pmc(pmc_msr, pmc_no);
    __asm__ __volatile__ (
        "call *%[func];"
        "movl %[pmc_no], %%ecx;"
        "rdpmc;"
        "shlq $32, %%rdx;"
        "orq %%rdx, %%rax;"
        :"=a"(eviction_count)
        :[func]"r"(cache), [pmc_no]"r"(pmc_no)
        :"%rdx", "%ecx"
    );

    /* pass back results */
    arg->rawr->analysis.eviction_count = eviction_count;
    return 0;
}

int __reverse_pred_cache(struct predecode_re *rawr, u32 pmc_msr, u32 pmc_no)
{
    
    /* map mempool for physically contingous memory regions large enough to 
       fill the predecode cache */
    u64 cache_0x66_size = PRED_CACHE_SIZE;
    u64 cache_0x67_size = PRED_CACHE_SIZE*64;
    
    size_t mempool_size = cache_0x66_size + cache_0x67_size + 1;
    char *mempool = kzalloc(mempool_size, GFP_KERNEL);
    if (!mempool) {
        meow(KERN_ERR, "couldnt alloc mempool");
        return -ENOMEM;
    }

    char *cache_0x66 = mempool;
    char *cache_0x67 = cache_0x66 + cache_0x66_size;

    /* copy in our code */
    for (u32 i = 0; i < cache_0x66_size/4; i++) {
        u32 base = i*4;
        cache_0x66[base] = 0x66;
        cache_0x66[base+1] = 0x83;
        cache_0x66[base+2] = 0xc0;
        cache_0x66[base+3] = 0x01;
    }

    for (u32 i = 0; i < cache_0x67_size/4; i++) {
        u32 base = i*4;
        cache_0x67[base] = 0x67;
        cache_0x67[base+1] = 0x0f;
        cache_0x67[base+2] = 0x1f;
        cache_0x67[base+3] = 0x00;
    }

    mempool[mempool_size - 1] = 0xc3;

    /* linux kernel will set xd in the pte of the mapped pages, so we
       unset this because we arent silly billies */
    rawr->func_ptrs.set_mem_x((unsigned long)mempool, num_pages(mempool_size));
    
    struct reverse_pred_cache arg = {
        .rawr = rawr,

        .predecode_cache = cache_0x66,

        .pmc_msr = pmc_msr,
        .pmc_no = pmc_no,
    };

    /* reverse engineer the predecode cache on the meow meow core rawrrr */
    int ret = stop_machine((cpu_stop_fn_t)__do_reverse_pred_cache, &arg, 
                           cpumask_of(smp_processor_id()));

    kfree(mempool);
    return ret;
}

int __do_analysis(struct predecode_re *rawr)
{
    /* disable, zero out and setup the pmc */
    u32 pmc_no = rawr->params.pmc_no;
    u32 evtsel_msr = IA32_PERFEVTSEL0 + pmc_no;
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

    __wrmsrl(evtsel_msr, evt.val);
    enable_pmc(pmc_no);

    /* reverse engineer the predecode cache */
    int ret = __reverse_pred_cache(rawr, pmc_msr, pmc_no);
    if (ret == 0) {
        meow(KERN_DEBUG, "guesstimated eviction count: %llu", 
             rawr->analysis.eviction_count);
    }

    /* disable and zero the pmc, were done now */
    disable_pmc(pmc_no);
    __wrmsrl(pmc_msr, 0);
    __wrmsrl(evtsel_msr, 0);

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

    set_memory_uc_t set_mem_uc = (set_memory_uc_t)kallsyms_ln("set_memory_uc");
    if (!set_mem_uc) {
        meow(KERN_ERR, "couldnt get set memory uc fuck");
        return -EFAULT;
    }

    flush_tlb_kernel_range_t __flush_tlb_kernel_range = 
        (flush_tlb_kernel_range_t)kallsyms_ln("flush_tlb_kernel_range");
    if (!__flush_tlb_kernel_range) {
        meow(KERN_ERR, "couldnt get flush_tlb_kernel_range frick bruh");
        return -EFAULT;
    }

    rawr->func_ptrs.kallsyms_ln = kallsyms_ln;
    rawr->func_ptrs.set_mem_x = set_mem_x;
    rawr->func_ptrs.set_mem_uc = set_mem_uc;
    rawr->func_ptrs.__flush_tlb_kernel_range = __flush_tlb_kernel_range;
        
    /* start analysis routine */
    return __do_analysis(rawr);
}