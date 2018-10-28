#ifndef _ASM_X86_QSPINLOCK_H
#define _ASM_X86_QSPINLOCK_H

#include <asm/cpufeature.h>
#include <asm-generic/qspinlock_types.h>
#include <asm/paravirt.h>

#define	queued_spin_unlock queued_spin_unlock
/**
 * queued_spin_unlock - release a queued spinlock
 * @lock : Pointer to queued spinlock structure
 *
 * A smp_store_release() on the least-significant byte.
 */
static inline void native_queued_spin_unlock(struct qspinlock *lock)
{
	smp_store_release((u8 *)lock, 0);
}

#if defined(CONFIG_PARAVIRT_SPINLOCKS) || defined(CONFIG_PARAVIRT_VCS)
DECLARE_PER_CPU_READ_MOSTLY(int, cpu_number);
#endif

#ifdef CONFIG_PARAVIRT_SPINLOCKS
extern void native_queued_spin_lock_slowpath(struct qspinlock *lock, u32 val);
extern void __pv_init_lock_hash(void);
extern void __pv_queued_spin_lock_slowpath(struct qspinlock *lock, u32 val);
extern void __raw_callee_save___pv_queued_spin_unlock(struct qspinlock *lock);

static inline void queued_spin_lock_slowpath(struct qspinlock *lock, u32 val)
{
	pv_queued_spin_lock_slowpath(lock, val);
#ifdef CONFIG_PARAVIRT_SPINLOCK_VCS
        vcpu_preempt_count(this_cpu_read(cpu_number), 1, KVM_SPN_HOLDER);
#endif
}

static inline void queued_spin_unlock(struct qspinlock *lock)
{
	pv_queued_spin_unlock(lock);
#ifdef CONFIG_PARAVIRT_SPINLOCK_VCS
        vcpu_preempt_count(this_cpu_read(cpu_number), -1, KVM_NO_CS);
#endif
}

#define vcpu_is_preempted vcpu_is_preempted
static inline bool vcpu_is_preempted(long cpu)
{
	return pv_vcpu_is_preempted(cpu);
}

#else
static inline void queued_spin_unlock(struct qspinlock *lock)
{
	native_queued_spin_unlock(lock);
#ifdef CONFIG_PARAVIRT_SPINLOCK_VCS
        vcpu_preempt_count(this_cpu_read(cpu_number), -1, KVM_NO_CS);
#endif
}
#endif

#ifdef CONFIG_PARAVIRT
#define virt_spin_lock virt_spin_lock
static inline bool virt_spin_lock(struct qspinlock *lock)
{
	if (!static_cpu_has(X86_FEATURE_HYPERVISOR))
		return false;

	/*
	 * On hypervisors without PARAVIRT_SPINLOCKS support we fall
	 * back to a Test-and-Set spinlock, because fair locks have
	 * horrible lock 'holder' preemption issues.
	 */

	do {
		while (atomic_read(&lock->val) != 0)
			cpu_relax();
	} while (atomic_cmpxchg(&lock->val, 0, _Q_LOCKED_VAL) != 0);

	return true;
}
#endif /* CONFIG_PARAVIRT */

#include <asm-generic/qspinlock.h>

#endif /* _ASM_X86_QSPINLOCK_H */
