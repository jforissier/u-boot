// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2011
 * Ilya Yanok, EmCraft Systems
 */
#include <cpu_func.h>
#include <asm/cache.h>
#include <linux/errno.h>
#include <linux/types.h>

#if !CONFIG_IS_ENABLED(SYS_DCACHE_OFF)
void _invalidate_dcache_all(void);
void invalidate_dcache_all(void)
{
	_invalidate_dcache_all();
}

void _flush_dcache_all(void);
void flush_dcache_all(void)
{
	_flush_dcache_all();
}

void _invalidate_dcache_range(unsigned long start, unsigned long stop);
void invalidate_dcache_range(unsigned long start, unsigned long stop)
{
	if (!check_cache_range(start, stop))
		return;
	_invalidate_dcache_range(start, stop);
}

void _flush_dcache_range(unsigned long start, unsigned long stop);
void flush_dcache_range(unsigned long start, unsigned long stop)
{
	if (!check_cache_range(start, stop))
		return;

	_flush_dcache_range(start, stop);
}
#else /* #if !CONFIG_IS_ENABLED(SYS_DCACHE_OFF) */
void invalidate_dcache_all(void)
{
}

void flush_dcache_all(void)
{
}
#endif /* #if !CONFIG_IS_ENABLED(SYS_DCACHE_OFF) */

/*
 * Stub implementations for l2 cache operations
 */

__weak void l2_cache_disable(void) {}

#if CONFIG_IS_ENABLED(SYS_THUMB_BUILD)
__weak void invalidate_l2_cache(void) {}
#endif

#if !CONFIG_IS_ENABLED(SYS_ICACHE_OFF)
/* Invalidate entire I-cache and branch predictor array */
void _invalidate_icache_all(void);
void invalidate_icache_all(void)
{
	_invalidate_icache_all();
}
#else
void invalidate_icache_all(void) {}
#endif

void enable_caches(void)
{
#if !CONFIG_IS_ENABLED(SYS_ICACHE_OFF)
	icache_enable();
#endif
#if !CONFIG_IS_ENABLED(SYS_DCACHE_OFF)
	dcache_enable();
#endif
}

int __weak pgprot_set_attrs(phys_addr_t addr, size_t size, enum pgprot_attrs perm)
{
	return -ENOSYS;
}
