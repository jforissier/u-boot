/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright 2025 Linaro Limited
 */

#include <linux/types.h>

#ifndef _UTHREAD_H_
#define _UTHREAD_H_

#ifdef CONFIG_UTHREAD

int uthread_create(void (*fn)(void *), void *arg, size_t stack_sz,
		   unsigned int grp_id);
bool uthread_schedule(void);
unsigned int uthread_grp_new_id(void);
bool uthread_grp_done(unsigned int grp_id);

#else

static inline int uthread_create(void (*fn)(void *), void *arg, size_t stack_sz,
				 unsigned int grp_id)
{
	fn(arg);
	return 0;
}

static inline bool uthread_schedule(void)
{
	return false;
}

static inline unsigned int uthread_grp_new_id(void)
{
	return 0;
}

static inline bool uthread_grp_done(unsigned int grp_id)
{
	return true;
}

#endif /* CONFIG_UTHREAD */
#endif /* _UTHREAD_H_ */
