#ifndef __LINUX_PARAVIRT_H
#define __LINUX_PARAVIRT_H

#ifdef CONFIG_PARAVIRT_VCS
#include <asm/paravirt.h>

#define inc_fake_preempt_count(_t)                                             \
        do {                                                                   \
                preempt_disable();                                             \
                smp_store_release(&current->vcs_preempt_state, 1);             \
                vcpu_inc_fake_preempt_count(smp_processor_id(), (_t));         \
                preempt_enable();                                              \
        } while (0)

#define dec_fake_preempt_count(_t)                                             \
        do {                                                                   \
                preempt_disable();                                             \
                vcpu_dec_fake_preempt_count(smp_processor_id(), (_t));         \
                smp_store_release(&current->vcs_preempt_state, 0);             \
                preempt_enable();                                              \
        } while (0)

#define dec_if_vcs_preempt_count(_t)                                           \
        do {                                                                   \
                if (READ_ONCE((_t)->vcs_preempt_state)) {                      \
                        int v =                                                \
                              vcpu_get_fake_preempt_count(smp_processor_id()); \
                        WRITE_ONCE((_t)->temp_vcs_count, v);                   \
                        vcpu_update_fake_preempt_count(smp_processor_id(),     \
                                                   (-1)*(_t)->temp_vcs_count,  \
                                                   KVM_NO_CS);                 \
                }                                                              \
        } while (0)

#define inc_if_vcs_preempt_count(_t)                                           \
        do {                                                                   \
                if (smp_load_acquire(&((_t)->vcs_preempt_state)))              \
                        vcpu_update_fake_preempt_count(smp_processor_id(),     \
                                                   (_t)->temp_vcs_count,       \
                                                   KVM_MUTEX_HOLDER);          \
        } while (0)

#define is_vcpu_preempted(_t) vcpu_is_preempted_other(_t)

#ifdef CONFIG_PARAVIRT_MUTEX_VCS
#define inc_mutex_count()                                                      \
        do {                                                                   \
                inc_fake_preempt_count(KVM_MUTEX_HOLDER);                      \
        } while (0)

#define dec_mutex_count()                                                      \
        do {                                                                   \
                dec_fake_preempt_count(KVM_NO_CS);                             \
        } while (0)
#else
#define inc_mutex_count() do { } while (0)
#define dec_mutex_count() do { } while (0)
#endif

#ifdef PARAVIRT_RWSEM_WR_VCS
#define inc_rwsem_wr_count()                                                   \
        do {                                                                   \
                inc_fake_preempt_count(KVM_RWSEM_WRITER);                      \
        } while (0)

#define dec_rwsem_wr_count()                                                   \
        do {                                                                   \
                dec_fake_preempt_count(KVM_NO_CS);                             \
        } while (0)
#else
#define inc_rwsem_wr_count() do { } while (0)
#define dec_rwsem_wr_count() do { } while (0)
#endif

#ifdef PARAVIRT_RWSEM_RD_VCS
#define inc_rwsem_rd_count()                                                   \
        do {                                                                   \
                inc_fake_preempt_count(KVM_RWSEM_READER);                      \
        } while (0)

#define dec_rwsem_rd_count()                                                   \
        do {                                                                   \
                dec_fake_preempt_count(KVM_NO_CS);                             \
        } while (0)
#else
#define inc_rwsem_rd_count() do { } while (0)
#define dec_rwsem_rd_count() do { } while (0)
#endif

#else

#define inc_mutex_count() do { } while (0)
#define dec_mutex_count() do { } while (0)
#define inc_rwsem_wr_count() do { } while (0)
#define dec_rwsem_wr_count() do { } while (0)
#define inc_rwsem_rd_count() do { } while (0)
#define dec_rwsem_rd_count() do { } while (0)
#define dec_if_vcs_preempt_count(_t) do { } while (0)
#define inc_if_vcs_preempt_count(_t) do { } while (0)
#define is_vcpu_preempted(_t) vcpu_is_preempted(_t)

#endif


#ifndef CONFIG_PARAVIRT_IPI
#define send_call_function_ipi_mask(_m) arch_send_call_function_ipi_mask(_m)
#define update_ipi_cpumask(_m) do { } while (0)
#endif


#endif /* __LINUX_PARAVIRT_H */
