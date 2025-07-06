#include "include/core.h"

/* 

* formatted this in intel syntax so
  i dont get yelled at by the skids

// returns event count for relevant pmc for 0x66 routine
u64 benchmark_routine1(u32 pmc_no);

nop DWORD PTR [rax+rax*1+0x0]
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
movzx ecx,ax
rdpmc
mov cr3, r8
shl rdi, 32
shl rdx, 32
or rsi, rdi
or rax, rdx
sub rax, rsi
ret 

benchmark_routine2 is throwaway

*/
u8 benchmark_routine1[] = 
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

u8 lcp_0x66[] = 
{
    0x66, 0x31, 0xC0, 0x66, 0x31, 0xC0, 0x66, 0x31, 
    0xC0, 0x66, 0x31, 0xC0, 0x66, 0x31, 0xC0, 0x66, 
    0x31, 0xC0, 0x66, 0x31, 0xC0, 0x66, 0x31, 0xC0, 
    0x66, 0x31, 0xC0, 0x66, 0x31, 0xC0, 0x66, 0x31, 
    0xC0, 0x66, 0x31, 0xC0, 0x66, 0x31, 0xC0, 0x66, 
    0x31, 0xC0, 0x66, 0x31, 0xC0, 0x66, 0x31, 0xC0, 
    0x66, 0x31, 0xC0, 0x66, 0x31, 0xC0, 0x66, 0x31, 
    0xC0, 0x66, 0x31, 0xC0, 0x66, 0x31, 0xC0, 0xC3 
};

u8 lcp_0x67[] = 
{ 
    0x67, 0x0F, 0x1F, 0x00, 0x67, 0x0F, 0x1F, 0x04, 
    0x00, 0x67, 0x0F, 0x1F, 0x00, 0x67, 0x0F, 0x1F, 
    0x04, 0x00, 0x67, 0x0F, 0x1F, 0x00, 0x67, 0x0F, 
    0x1F, 0x04, 0x00, 0x67, 0x0F, 0x1F, 0x00, 0x67, 
    0x0F, 0x1F, 0x04, 0x00, 0x67, 0x0F, 0x1F, 0x00, 
    0x67, 0x0F, 0x1F, 0x04, 0x00, 0x67, 0x0F, 0x1F, 
    0x00, 0x67, 0x0F, 0x1F, 0x04, 0x00, 0x67, 0x0F, 
    0x1F, 0x00, 0x67, 0x0F, 0x1F, 0x04, 0x00, 0xC3 
};

int __do_reverse_pred_cache(struct reverse_pred_cache *arg)
{
    /* we pass shit via struct coz cant alloc mem here 
       or crash bc calling stop_machine */

    char *cache1 = arg->predecode_cache1;
    char *cache2 = arg->predecode_cache2;

    u32 pmc_msr = arg->pmc_msr;
    u32 pmc_no = arg->pmc_no;

    u64 eviction_count = 0;

    /* re whatever we need to re however we need */


    u64 count1 = 0;
    zero_enabled_pmc(pmc_msr, pmc_no);
    __asm__ __volatile__ (
        "call %[func];"
        "movl %[pmc_no], %%ecx;"
        "rdpmc;"
        "shlq $32, %%rdx;"
        "orq %%rdx, %%rax;"
        :"=a"(count1)
        :[func]"r"(cache1), [pmc_no]"r"(pmc_no)
        :"%rdx", "%ecx"
    );

    u64 count2 = 0;
    zero_enabled_pmc(pmc_msr, pmc_no);
    __asm__ __volatile__ (
        "call %[func];"
        "movl %[pmc_no], %%ecx;"
        "rdpmc;"
        "shlq $32, %%rdx;"
        "orq %%rdx, %%rax;"
        :"=a"(count2)
        :[func]"r"(cache2), [pmc_no]"r"(pmc_no)
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
    size_t mempool_size = (PRED_CACHE_SIZE * 4);
    char *mempool = kzalloc(mempool_size, GFP_KERNEL);
    if (!mempool) {
        meow(KERN_ERR, "couldnt alloc mempool");
        return -ENOMEM;
    }

    char *predecode_cache1 = mempool;
    char *predecode_cache2 = predecode_cache1 + (PRED_CACHE_SIZE*2) + 1;

    u8 instr_0x66[] = {0x66, 0x83, 0xc0, 0x01}; // add ax, 1
    u8 instr_0x67[] = {0x67, 0x0f, 0x1f, 0x00}; // nop dword ptr [eax]

    for (u32 i = 0; i < PRED_CACHE_SIZE/4; i++) {
        u32 base = i*4;
        predecode_cache1[base] = instr_0x66[0];
        predecode_cache1[base+1] = instr_0x66[1];
        predecode_cache1[base+2] = instr_0x66[2];
        predecode_cache1[base+3] = instr_0x66[3];
    }

    for (u32 i = 0; i < PRED_CACHE_SIZE/4; i++) {
        u32 base = i*4;
        predecode_cache2[base] = instr_0x67[0];
        predecode_cache2[base+1] = instr_0x67[1];
        predecode_cache2[base+2] = instr_0x67[2];
        predecode_cache2[base+3] = instr_0x67[3];
    }

    *(predecode_cache1 + PRED_CACHE_SIZE) = 0xc3;
    *(predecode_cache2 + PRED_CACHE_SIZE) = 0xc3;

    /* linux kernel will set xd in the pte of the mapped pages, so we
       unset this because we arent silly billies */
    rawr->func_ptrs.set_mem_x((unsigned long)mempool, mempool_size/PAGE_SIZE);
    
    struct reverse_pred_cache arg = {
        .rawr = rawr,

        .predecode_cache1 = predecode_cache1,
        .predecode_cache2 = predecode_cache2,

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