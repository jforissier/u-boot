/* SPDX-License-Identifier: GPL-2.0+ */

#include <config.h>
#include <linux/linkage.h>

ENTRY_OWN_SECTION(_get_cr_hyp)
	mrc	p15, 4, r0, c1, c0, 0 @ get CR
	bx	lr
ENDPROC_OWN_SECTION(_get_cr_hyp)

ENTRY_OWN_SECTION(_get_cr_nohyp)
	mrc	p15, 0, r0, c1, c0, 0 @ get CR
	bx	lr
ENDPROC_OWN_SECTION(_get_cr_nohyp)

ENTRY_OWN_SECTION(_set_cr_hyp)
	mcr	p15, 4, r0, c1, c0, 0 @ set CR
	bx	lr
ENDPROC_OWN_SECTION(_set_cr_hyp)

ENTRY_OWN_SECTION(_set_cr_nohyp)
	mcr	p15, 0, r0, c1, c0, 0 @ set CR
	bx	lr
ENDPROC_OWN_SECTION(_set_cr_nohyp)

ENTRY_OWN_SECTION(_invalidate_l2_cache)
	mcr	p15, 1, r0, c15, c11, 0 @ invl l2 cache
	bx	lr
ENDPROC_OWN_SECTION(_invalidate_l2_cache)

#if defined(CONFIG_SYS_ARM_MMU) && defined(CONFIG_ARMV7_LPAE) && \
    __LINUX_ARM_ARCH__ != 4

ENTRY_OWN_SECTION(_mmu_helper_lpae_hyp)
	/* Set HTCR to enable LPAE */
	mcr	p15, 4, r0, c2, c0, 2
	/* Set HTTBR0 */
	mov	r4, #0
	mcrr	p15, 4, r1, r4, c2
	/* Set HMAIR */
	mcr	p15, 4, r3, c10, c2, 0
	bx	lr
ENDPROC_OWN_SECTION(_mmu_helper_lpae_hyp)

ENTRY_OWN_SECTION(_mmu_helper_lpae_nohyp)
	/* Set TTBCR to enable LPAE */
	mcr	p15, 0, r0, c2, c0, 2
	/* Set 64-bit TTBR0 */
	mov	r4, #0
	mcrr	p15, 0, r1, r4, c2
	/* Set MAIR */
	mcr	p15, 0, r3, c10, c2, 0
	bx lr
ENDPROC_OWN_SECTION(_mmu_helper_lpae_nohyp)

#elif defined(CONFIG_CPU_V7A)

ENTRY_OWN_SECTION(_mmu_helper_nolpae_hyp)
	/* Set HTCR to disable LPAE */
	mov	r1, #0
	mcr	p15, 4, r1, c2, c0, 2
	/* Set TTBR0 */
	mcr	p15, 0, r0, c2, c0, 0
	bx	lr
ENDPROC_OWN_SECTION(_mmu_helper_nolpae_hyp)

ENTRY_OWN_SECTION(_mmu_helper_nolpae_nohyp)
	/* Set TTBCR to disable LPAE */
	mov	r1, #0
	mcr	p15, 0, r1, c2, c0, 2
	/* Set TTBR0 */
	mcr	p15, 0, r0, c2, c0, 0
	bx	lr
ENDPROC_OWN_SECTION(_mmu_helper_nolpae_nohyp)

#else

ENTRY_OWN_SECTION(_mmu_helper_pt)
	mcr	p15, 0, r0, c2, c0, 0
	bx	lr
ENDPROC_OWN_SECTION(_mmu_helper_pt)

#endif

#if !(defined(CONFIG_SYS_ICACHE_OFF) && defined(CONFIG_SYS_DCACHE_OFF)) && \
    defined(CONFIG_SYS_ARM_MMU)

ENTRY_OWN_SECTION(_set_dacr)
	mcr p15, 0, r0, c3, c0, 0
	bx	lr
ENDPROC_OWN_SECTION(_set_dacr)

#endif
