// SPDX-License-Identifier: GPL-2.0-or-later
#include <asm/paravirt.h>
#include <asm/asm-offsets.h>
#include <linux/stringify.h>

DEF_NATIVE(pv_irq_ops, irq_disable, "cli");
DEF_NATIVE(pv_irq_ops, irq_enable, "sti");
DEF_NATIVE(pv_irq_ops, restore_fl, "pushq %rdi; popfq");
DEF_NATIVE(pv_irq_ops, save_fl, "pushfq; popq %rax");
DEF_NATIVE(pv_mmu_ops, read_cr2, "movq %cr2, %rax");
DEF_NATIVE(pv_mmu_ops, read_cr3, "movq %cr3, %rax");
DEF_NATIVE(pv_mmu_ops, write_cr3, "movq %rdi, %cr3");
DEF_NATIVE(pv_mmu_ops, flush_tlb_single, "invlpg (%rdi)");
DEF_NATIVE(pv_cpu_ops, wbinvd, "wbinvd");

DEF_NATIVE(pv_cpu_ops, usergs_sysret64, "swapgs; sysretq");
DEF_NATIVE(pv_cpu_ops, swapgs, "swapgs");

DEF_NATIVE(, mov32, "mov %edi, %eax");
DEF_NATIVE(, mov64, "mov %rdi, %rax");

#if defined(CONFIG_PARAVIRT_SPINLOCKS)
DEF_NATIVE(pv_lock_ops, queued_spin_unlock, "movb $0, (%rdi)");
DEF_NATIVE(pv_lock_ops, vcpu_is_preempted, "xor %rax, %rax");
#endif

/* eCS */
#if defined(CONFIG_PARAVIRT_VCS)
DEF_NATIVE(pv_sched_ops, vcpu_is_preempted, "xor %rax, %rax");
DEF_NATIVE(pv_sched_ops, vcpu_get_fake_preempt_count, "xor %rax, %rax");
DEF_NATIVE(pv_sched_ops, pcpu_is_overloaded, "xor %eax, %eax");
DEF_NATIVE(pv_sched_ops, vcpu_preempt_count, "");
DEF_NATIVE(pv_sched_ops, vcpu_fake_preempt_count, "");
#endif
/*******/

unsigned paravirt_patch_ident_32(void *insnbuf, unsigned len)
{
	return paravirt_patch_insns(insnbuf, len,
				    start__mov32, end__mov32);
}

unsigned paravirt_patch_ident_64(void *insnbuf, unsigned len)
{
	return paravirt_patch_insns(insnbuf, len,
				    start__mov64, end__mov64);
}

extern bool pv_is_native_spin_unlock(void);
extern bool pv_is_native_vcpu_is_preempted(void);
/* eCS */
extern bool pv_is_native_pcpu_is_overloaded(void);
extern bool pv_is_native_vcpu_is_preempted_other(void);
extern bool pv_is_native_vcpu_get_fake_preempt_count(void);
extern bool pv_is_native_vcpu_preempt_count(void);
extern bool pv_is_native_vcpu_fake_preempt_count(void);
/*******/

unsigned native_patch(u8 type, u16 clobbers, void *ibuf,
		      unsigned long addr, unsigned len)
{
	const unsigned char *start, *end;
	unsigned ret;

#define PATCH_SITE(ops, x)					\
		case PARAVIRT_PATCH(ops.x):			\
			start = start_##ops##_##x;		\
			end = end_##ops##_##x;			\
			goto patch_site
	switch(type) {
		PATCH_SITE(pv_irq_ops, restore_fl);
		PATCH_SITE(pv_irq_ops, save_fl);
		PATCH_SITE(pv_irq_ops, irq_enable);
		PATCH_SITE(pv_irq_ops, irq_disable);
		PATCH_SITE(pv_cpu_ops, usergs_sysret64);
		PATCH_SITE(pv_cpu_ops, swapgs);
		PATCH_SITE(pv_mmu_ops, read_cr2);
		PATCH_SITE(pv_mmu_ops, read_cr3);
		PATCH_SITE(pv_mmu_ops, write_cr3);
		PATCH_SITE(pv_mmu_ops, flush_tlb_single);
		PATCH_SITE(pv_cpu_ops, wbinvd);
#if defined(CONFIG_PARAVIRT_SPINLOCKS)
		case PARAVIRT_PATCH(pv_lock_ops.queued_spin_unlock):
			if (pv_is_native_spin_unlock()) {
				start = start_pv_lock_ops_queued_spin_unlock;
				end   = end_pv_lock_ops_queued_spin_unlock;
				goto patch_site;
			}
			goto patch_default;

		case PARAVIRT_PATCH(pv_lock_ops.vcpu_is_preempted):
			if (pv_is_native_vcpu_is_preempted()) {
				start = start_pv_lock_ops_vcpu_is_preempted;
				end   = end_pv_lock_ops_vcpu_is_preempted;
/* eCS */
				goto patch_default;
			}
			goto patch_default;
#endif
#if defined(CONFIG_PARAVIRT_VCS)
		case PARAVIRT_PATCH(pv_sched_ops.vcpu_is_preempted):
			if (pv_is_native_vcpu_is_preempted_other()) {
				start = start_pv_sched_ops_vcpu_is_preempted;
				end   = end_pv_sched_ops_vcpu_is_preempted;
				goto patch_default;
			}
			goto patch_default;

                case PARAVIRT_PATCH(pv_sched_ops.pcpu_is_overloaded):
			if (pv_is_native_pcpu_is_overloaded()) {
				start = start_pv_sched_ops_pcpu_is_overloaded;
				end   = end_pv_sched_ops_pcpu_is_overloaded;
/*******/
				goto patch_site;
			}
			goto patch_default;

/* eCS */
		case PARAVIRT_PATCH(pv_sched_ops.vcpu_get_fake_preempt_count):
			if (pv_is_native_vcpu_get_fake_preempt_count()) {
				start = start_pv_sched_ops_vcpu_get_fake_preempt_count;
				end   = end_pv_sched_ops_vcpu_get_fake_preempt_count;
				goto patch_default;
			}
			goto patch_default;

                case PARAVIRT_PATCH(pv_sched_ops.vcpu_preempt_count):
                        if (pv_is_native_vcpu_preempt_count()) {
                                start = start_pv_sched_ops_vcpu_preempt_count;
                                end = end_pv_sched_ops_vcpu_preempt_count;
                                goto patch_default;
                        }
                        goto patch_default;

                case PARAVIRT_PATCH(pv_sched_ops.vcpu_fake_preempt_count):
                        if (pv_is_native_vcpu_fake_preempt_count()) {
                                start = start_pv_sched_ops_vcpu_fake_preempt_count;
                                end = end_pv_sched_ops_vcpu_fake_preempt_count;
                                goto patch_default;
                        }
                        goto patch_default;
/*******/
#endif

	default:
patch_default: __maybe_unused
		ret = paravirt_patch_default(type, clobbers, ibuf, addr, len);
		break;

patch_site:
		ret = paravirt_patch_insns(ibuf, len, start, end);
		break;
	}
#undef PATCH_SITE
	return ret;
}
