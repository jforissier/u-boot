/* SPDX-License-Identifier: GPL-2.0+ */

#include <config.h>
#include <linux/linkage.h>

ENTRY_OWN_SECTION(_cache_flush)
	mcr	p15, 0, r0, c7, c7
	bx	lr
ENDPROC_OWN_SECTION(_cache_flush)

ENTRY_OWN_SECTION(_invalidate_dcache_all)
	mcr	p15, 0, r0, c7, c6, 0
	bx	lr
ENDPROC_OWN_SECTION(_invalidate_dcache_all)

ENTRY_OWN_SECTION(_flush_dcache_all)
0:
	mrc	p15, 0, r15, c7, c14, 3
	bne	0b
	mcr	p15, 0, r0, c7, c10, 4
	bx	lr
ENDPROC_OWN_SECTION(_flush_dcache_all)

ENTRY_OWN_SECTION(_invalidate_dcache_range)
0:
	mcr	p15, 0, r0, c7, c6, 1
	add	r0, r0, #CONFIG_SYS_CACHELINE_SIZE
	cmp	r0, r1
	blt	0b
	bx	lr
ENDPROC_OWN_SECTION(_invalidate_dcache_range)

ENTRY_OWN_SECTION(_flush_dcache_range)
0:
	mcr	p15, 0, r0, c7, c14, 1
	add	r0, r0, #CONFIG_SYS_CACHELINE_SIZE
	cmp	r0, r1
	blt	0b
	mcr	p15, 0, r0, c7, c10, 4
	bx	lr
ENDPROC_OWN_SECTION(_flush_dcache_range)

ENTRY_OWN_SECTION(_invalidate_icache_all)
	mcr	p15, 0, r0, c7, c5, 0
	bx	lr
ENDPROC_OWN_SECTION(_invalidate_icache_all)
