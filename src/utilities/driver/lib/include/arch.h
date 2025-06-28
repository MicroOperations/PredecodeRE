#ifndef _ARCH_H_
#define _ARCH_H_

#include <linux/types.h>
#include <asm/paravirt.h>

/* took me a couple of hours doing all dis shit 
   from scratch
   
   scratch ^~_~^ meow purrr rawrrr */

typedef union
{
    u64 val;
    struct
    {
        u64 pe : 1;
        u64 mp : 1;
        u64 em : 1;
        u64 ts : 1;
        u64 et : 1;
        u64 ne : 1;
        u64 reserved0 : 10;
        u64 wp : 1;
        u64 reserved1 : 1;
        u64 am : 1;
        u64 reserved2 : 10;
        u64 nw : 1;
        u64 cd : 1;
        u64 pg : 1;
        u64 undefined0 : 32;
    } fields;
} cr0_t;

typedef union 
{
    u64 val;
    struct
    {
        u64 vme : 1;
        u64 pvi : 1;
        u64 tsd : 1;
        u64 de : 1;
        u64 pse : 1;
        u64 pae : 1;
        u64 mce : 1;
        u64 pge : 1;
        u64 pce : 1;
        u64 osfxr : 1;
        u64 osxmmexcpt : 1;
        u64 umip : 1;
        u64 la57 : 1;
        u64 vmxe : 1;
        u64 smxe : 1;
        u64 reserved0 : 1;
        u64 fsgsbase : 1;
        u64 pcide : 1;
        u64 osxsave : 1;
        u64 kl : 1;
        u64 smep : 1;
        u64 smap : 1;
        u64 pke : 1;
        u64 cet : 1;
        u64 pks : 1;
        u64 uintr : 1;
        u64 reserved1 : 2;
        u64 lam_sup : 1;
    } fields;
} cr4_t;

#define IA32_SPEC_CTRL 0x48
typedef union
{
    u64 val;
    struct
    {
        u64 ibrs : 1;  
        u64 stibp : 1; 
        u64 ssbd : 1;
        u64 ipred_dis_u : 1;
        u64 ipred_dis_s : 1;
        u64 rrsba_dis_u : 1;
        u64 rrsba_dis_s : 1;
        u64 psfd : 1;   
        u64 ddpd_u : 1; 
        u64 reserved1 : 1;
        u64 bhi_dis_s : 1;
        u64 reserved2 : 53;
    } fields;
} ia32_spec_ctrl_t;

#define IA32_PRED_CMD 0x49
typedef union
{
    u64 val;
    struct
    {
        u64 ibpb : 1; 
        u64 reserved : 63;
    } fields;
} ia32_pred_t;

#define IA32_MISC_ENABLE 0x1A0
typedef union
{
    u64 val;
    struct
    {
        u64 fast_strings_enable : 1;
        u64 reserved1 : 2;
        u64 tcc : 1;
        u64 reserved2 : 3;
        u64 perfmon_enable : 1;
        u64 reserved3 : 3;
        u64 bts_unavailable : 1;
        u64 pebs_unavailable : 1;
        u64 reserved4 : 3;
        u64 est_enabled : 1;
        u64 reserved5 : 1;
        u64 monitor_fsm_enabled : 1;
        u64 reserved6 : 3;
        u64 limit_cpuid_maxval : 1;
        u64 xtrp_message_disable : 1;
        u64 reserved0 : 20;
    } fields;
} ia32_misc_enable_t;

#define IA32_DEBUG_CTL 0x1D9
typedef union
{
    u64 val;
    struct
    {
        u64 lbr : 1;
        u64 btf : 1;
        u64 bld : 1;
        u64 reserved1 : 3;
        u64 tr : 1;
        u64 bts : 1;
        u64 btint : 1;
        u64 bts_off_os : 1;
        u64 bts_off_usr : 1;
        u64 freeze_lbrs_on_pmi : 1;
        u64 freeze_perfmon_on_pmi : 1;
        u64 enable_unicore_pmi : 1;
        u64 freeze_while_smm : 1;
        u64 rtm_debug : 1;
        u64 reserved2 : 48;
    } fields;
} ia32_debug_ctl_t;

#define IA32_ARCH_CAPABILITIES 0x10A
typedef union
{
    u64 val;
    struct
    {
        u64 rdcl_no : 1;
        u64 ibrs_all : 1;
        u64 rsba : 1;
        u64 skip_l1dfl_vmentry : 1;
        u64 ssb_no : 1;
        u64 mds_no : 1;
        u64 if_pschange_mc_no : 1;
        u64 tsx_ctrl : 1;
        u64 taa_no : 1;
        u64 mcu_control : 1;
        u64 misc_package_ctls : 1;
        u64 energy_filtering_ctl : 1;
        u64 doitm : 1;
        u64 sbdr_ssdp_no : 1;
        u64 fbsdp_no : 1;
        u64 psdp_no : 1;
        u64 mcu_enumeration : 1;
        u64 fb_clear : 1;
        u64 fb_clear_ctrl : 1;
        u64 rrsba : 1;
        u64 bhi_no : 1;
        u64 xapic_disable_status : 1;
        u64 mcu_extended_service : 1;
        u64 overclocking_status : 1;
        u64 pbrsb_no : 1;
        u64 gds_ctrl : 1;
        u64 gds_no : 1;
        u64 rfds_no : 1;
        u64 rfds_clear : 1;
        u64 ign_umonitor_support : 1;
        u64 mon_umon_mitg_support : 1;
        u64 reserved0 : 33;
    } fields;
} ia32_arch_capabilities_t;

#define IA32_PERF_CAPABILITIES 0x345
typedef union
{
    u64 val;
    struct
    {
        u64 lbr_format : 6;
        u64 pebs_trap : 1;
        u64 pebs_save_arch_regs : 1;
        u64 pebs_record_format : 4;
        u64 freeze_smm_supported : 1;
        u64 fw_ia32_a_pmcx : 1;
        u64 pebs_baseline : 1;
        u64 perf_metrics_available : 1;
        u64 pebs_written_to_intel_pt_ts : 1;
        u64 pebs_retire_latency_output : 1;
        u64 tsx_address : 1;
        u64 rdpmc_metrics_clear : 1;
        u64 reserved0 : 44;
    } fields;
} ia32_perf_capabilities_t;

#define IA32_LBR_CTL 0x14CE
typedef union
{
    u64 val;
    struct
    {
        u64 lbr_enabled : 1;
        u64 os : 1;
        u64 usr : 1;
        u64 call_stack : 1;
        u64 reserved1 : 12;
        u64 cond : 1;
        u64 near_rel_jmp : 1;
        u64 near_ind_jmp : 1;
        u64 near_rel_call : 1;
        u64 near_ind_call : 1;
        u64 near_ret : 1;
        u64 other_branch : 1;
        u64 reserved2 : 41;
    } fields;
} ia32_lbr_ctl_t;

#define IA32_LBR_DEPTH 0x14CF

typedef enum
{
    IA32_LBR_0_INFO = 0x1200,
    IA32_LBR_31_INFO = 0x121F,
    
    IA32_LBR_0_FROM_IP = 0x1500,
    IA32_LBR_31_FROM_IP = 0x151F,

    IA32_LBR_0_TO_IP = 0x1600,
    IA32_LBR_31_TO_IP = 0x161F,
} lbr_stack_entry;

typedef union
{
    u64 val;
    struct
    {
        u64 cyc_cnt : 16;
        u64 undefined : 40;
        u64 br_type : 4;
        u64 cyc_cnt_valid : 1;
        u64 tsx_abort : 1;
        u64 in_tsx : 1;
        u64 mispred : 1;
    } fields;
} ia32_lbr_info_t;

typedef enum 
{
    COND,
    NEAR_JMP_IND,
    NEAR_JMP_REL,
    NEAR_CALL_IND,
    NEAR_CALL_REL,
    NEAR_RET,
    RESERVED,
    OTHER,
    UNKNOWN,
} lbr_branch_type;

#define IA32_PERF_GLOBAL_STATUS 0x38E
#define IA32_PERF_GLOBAL_STATUS_RESET 0x390
#define IA32_PERF_GLOBAL_STATUS_SET 0x391
#define IA32_PERF_GLOBAL_STATUS_INUSE 0x392
typedef union
{
    u64 val;
    struct
    {
        u64 ovf_pmcn : 31;
        u64 reserved0 : 1;
        u64 ovf_fixedctrm : 15;
        u64 reserved1 : 1;
        u64 ovf_perf_metrics : 1;
        u64 reserved2 : 6;
        u64 ToPA_filled_pmi : 1;
        u64 reserved3 : 2;
        u64 lbr_frozen : 1;
        u64 pmc_frozen : 1;
        u64 asci : 1;
        u64 ovf_uncore : 1;
        u64 ovf_ds_buf : 1;
        u64 cond_changed : 1;
    } fields;
} ia32_perf_global_status_t;

#define IA32_PERF_GLOBAL_CTRL 0x38F
typedef union
{
    u64 val;
    struct
    {
        u64 enable_pmcn : 31;
        u64 reserved0 : 1;
        u64 enable_fixedctrm : 15;
        u64 reserved1 : 1;
        u64 enable_perf_metrics : 1;
        u64 reserved2 : 15;
    } fields;
} ia32_perf_global_ctrl_t;

/* legacy pmc interface */

#define IA32_PMC0 0xC1
#define IA32_A_PMC0 0x4C1
#define IA32_PERFEVTSEL0 0x186
typedef union
{
    u64 val;
    struct
    {
        u64 evtsel : 8;
        u64 umask : 8;
        u64 usr : 1;
        u64 os : 1;
        u64 edge : 1;
        u64 pc : 1;
        u64 ovf_pmi : 1;
        u64 anythread : 1;
        u64 enable_pmc : 1;
        u64 inv : 1;
        u64 cmask : 8;
        u64 reserved0 : 32;
    } fields;
} ia32_perfevtsel_t;

/* 
    use these for tmam, heres a note to myself so I dont frickin forget 
    what each of em r for

    * IA32_FIXED_CTR0 - counts instructions retired 
    * IA32_FIXED_CTR1 - counts cycles when the core is unhalted
    * IA32_FIXED_CTR2 - counts at tsc rate when core is unhalted
    * IA32_FIXED_CTR3 - counts topdown slots for an unhalted core
    * IA32_FIXED_CTR4 - counts topdown slots wasted bcoz of bad speculation
    * IA32_FIXED_CTR5 - counts topdown slots wasted bc of frontend delays
    * IA32_FIXED_CTR6 - counts topdown slots successfully retired
*/
typedef enum 
{
    IA32_FIXED_CTR0 = 0x309, 
    IA32_FIXED_CTR1, 
    IA32_FIXED_CTR2,
    IA32_FIXED_CTR3, 
    IA32_FIXED_CTR4, 
    IA32_FIXED_CTR5, 
    IA32_FIXED_CTR6, 
} ia32_fixed_ctr_addr_t;

#define IA32_FIXED_CTR_CTRL 0x38D
typedef union
{
    u64 val;
    struct 
    {
        u64 fixedctr0_os : 1;
        u64 fixedctr0_usr : 1;
        u64 fixedctr0_anythread : 1;
        u64 fixedctr0_ovf_pmi : 1;
        u64 fixedctr1_os : 1;
        u64 fixedctr1_usr : 1;
        u64 fixedctr1_anythread : 1;
        u64 fixedctr1_ovf_pmi : 1;
        u64 fixedctr2_os : 1;
        u64 fixedctr2_usr : 1;
        u64 fixedctr2_anythread : 1;
        u64 fixedctr2_ovf_pmi : 1;
        u64 fixedctr3_os : 1;
        u64 fixedctr3_usr : 1;
        u64 reserved0 : 1;
        u64 fixedctr3_ovf_pmi : 1;
        u64 reserved1 : 48;
    } fields;
} ia32_fixed_ctr_ctrl_t;

/* modern pmc interface */

#define IA32_PMC_GP0_CTR 0x1900
#define IA32_PMC_FX0_CTR 0x1980
typedef union
{
    u64 val;
    struct
    {
        u64 reload_val : 48;
        u64 reserved0 : 16;
    } fields;
} ia32_pmc_gp_ctr_t;
typedef ia32_pmc_gp_ctr_t ia32_pmc_fx_ctr_t;

#define IA32_PMC_GP0_CFG_A 0x1901
typedef union
{
    u64 val;
    struct
    {
        u64 evtsel : 8;
        u64 umask1 : 8;
        u64 usr : 1;
        u64 os : 1;
        u64 edge : 1;
        u64 reserved0 : 1;
        u64 ovf_pmi : 1;
        u64 anythread : 1;
        u64 enable_pmc : 1;
        u64 inv : 1;
        u64 cmask : 8;
        u64 reserved1 : 3;
        u64 enable_lbr_log : 1;
        u64 equal : 1;
        u64 reserved2 : 3;
        u64 umask2 : 8;
        u64 reserved3 : 16;
    } fields;
} ia32_pmc_gp_cfg_a_t;
typedef ia32_pmc_gp_cfg_a_t ia32_pmc_fx_cfg_a_t;

#define IA32_PMC_GP2_CFG_B 0x190A
#define IA32_PMC_FX0_CFG_B 0x1982
typedef union
{
    u64 val;
    struct 
    {
        u64 reserved0 : 2;
        u64 reload_pmc2 : 1;
        u64 reload_pmc3 : 1;
        u64 reload_pmc4 : 1;
        u64 reload_pmc5 : 1;
        u64 reload_pmc6 : 1;
        u64 reload_pmc7 : 1;
        u64 reserved1 : 24;
        u64 reload_fc0 : 1;
        u64 reload_fc1 : 1;
        u64 reserved2 : 14;
        u64 metrics_clear : 1;
        u64 reserved3 : 15;
    } fields;
} ia32_pmc_gp_cfg_b_t;
typedef ia32_pmc_gp_cfg_b_t ia32_pmc_fx_cfg_b_t;

#define IA32_PMC_GP0_CFG_C 0x1903
#define IA32_PMC_FX0_CFG_C 0x1983
typedef union 
{
    u64 val;
    struct 
    {
        u64 reload_val : 32;
        u64 reserved0 : 32;
    } fields;
} ia32_pmc_gp_cfg_c_t;
typedef ia32_pmc_gp_cfg_c_t ia32_pmc_fx_cfg_c_t;

/* events and umasks */

#define UNHALTED_CORE_CYCLES_EVTSEL 0x3C 
#define UNHALTED_CORE_CYCLES_UMASK 0x0

#define INSTRUCTIONS_RETIRED_EVTSEL 0xC0
#define INSTRUCTIONS_RETIRED_UMASK 0x0

#define UNHALTED_REF_CYCLES_EVTSEL 0x3C
#define UNHALTED_REF_CYCLES_UMASK 0x1

#define LLC_REF_EVTSEL 0x2E
#define LLC_REF_UMASK 0x4F

#define LLC_MISSES_EVTSEL 0x2E
#define LLC_MISSES_UMASK 0x41

#define BRANCH_INSTR_RETIRED_EVTSEL 0xC4
#define BRANCH_INSTR_RETIRED_UMASK 0x0

#define ALL_BRANCH_INSTR_EVTSEL 0xC5
#define ALL_BRANCH_INSTR_UMASK 0x0

#define TOPDOWN_SLOTS_EVTSEL 0xA4
#define TOPDOWN_SLOTS_UMASK 0x1

#define TOPDOWN_BACKEND_BOUND_EVTSEL 0xA4
#define TOPDOWN_BACKEND_BOUND_UMASK 0x2

#define TOPDOWN_BAD_SPECULATION_EVTSEL 0x73
#define TOPDOWN_BAD_SPECULATION_UMASK 0x0

#define TOPDOWN_FRONTEND_BOUND_EVTSEL 0x9C
#define TOPDOWN_FRONTEND_BOUND_UMASK 0x1

#define TOPDOWN_RETIRING_EVTSEL 0xC2
#define TOPDOWN_RETIRING_UMASK 0x2

#define LBR_INSERTS_EVTSEL 0xE4
#define LBR_INSERTS_UMASK 0x1

#define PRED_WRONG_EVTSEL 0xe9
#define PRED_WRONG_UMASK 0x1

#define ICACHE_MISSES_EVTSEL 0x80
#define ICACHE_MISSES_UMASK 0x02

/* cpuid bullshit */

#define CPUID_MANUFACTURER_ID 0
#define GENUINE_INTEL_EBX 0x756E6547
#define GENUINE_INTEL_ECX 0x6C65746E
#define GENUINE_INTEL_EDX 0x49656E69

#define CPUID_FEATURE_BITS 1

#define CPUID_EXTENDED_FEATURES 7

#define CPUID_ARCH_PERFMON 0x0A
typedef union 
{
    u32 val;
    struct 
    {
        u32 version_id : 8;
        u32 no_pmcs : 8;
        u32 bitwidth : 8;
        u32 no_arch_events_supported : 8;
    } fields;
} arch_perfmon_a_t;

/* basicallyyyy the bits enumerated in ebx r of 
   'negative polarity' so if theyre set its 
   unsupported so ye, but for extended its 
   positive polarity which is a lil weird lol */
typedef union 
{
    u32 val;
    struct
    {
        u32 unhalted_core_cycles : 1;
        u32 instructions_retired : 1;
        u32 unhalted_ref_cycles : 1;
        u32 llc_ref : 1;
        u32 llc_misses : 1;
        u32 branch_instr_retired : 1;
        u32 branch_miss_retired : 1;
        u32 topdown_slots : 1;
        u32 topdown_backend_bound : 1;
        u32 topdown_bad_speculation : 1;
        u32 topdown_frontend_bound : 1;
        u32 topdown_retiring : 1;
        u32 lbr_inserts : 1;
        u32 reserved0 : 19;
    } fields;
} arch_perfmon_b_t;

typedef union
{
    u32 val;
    struct
    {
       u32 fixed_func_pmcs : 5;
       u32 bitwidth : 8;
       u32 reserved0 : 2;
       u32 anythread_deprecation : 1;
       u32 reserved1 : 16;
    } fields;
} arch_perfmon_d_t;

#define CPUID_ARCH_PERFMON_EXT 0x23

typedef union 
{
    u32 val;
    struct
    {
        u32 umask2_supported : 1;
        u32 eq_supported : 1;
        u32 reserved0 : 30;
    } fields;
} arch_perfmon_ext_0b_t;

typedef union
{
    u32 val;
    struct 
    {   
        u32 no_tma_slots : 8;
        u32 reserved0 : 24;
    } fields;
} arch_perfmon_ext_0c_t;

/* helpers */

inline u64 __rdmsrl(u32 msr);
inline void __wrmsrl(u32 msr, u64 val);

inline void __prefetcht0(void *addr);
inline void __prefetcht1(void *addr);
inline void __prefetcht2(void *addr);
inline void __prefetchnta(void *addr);
inline void __prefetchw(void *addr);

inline bool is_cpu_intel(void);

inline bool is_arch_lbr_supported(void);
inline bool is_arch_perfmon_ext_supported(void);
inline bool is_invd_prevention_supported(void);
inline bool is_cldemote_supported(void);

inline void disable_perf_metrics(void);

inline bool fw_a_pmc_supported(u32 pmc_no);
inline void enable_pmc(u32 pmc_no);
inline void disable_pmc(u32 pmc_no);

inline void zero_enabled_pmc(u32 pmc_msr, u32 pmc_no);

#endif