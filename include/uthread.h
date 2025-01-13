/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright 2025 Linaro Limited
 */

#include <linux/types.h>

#ifndef _UTHREAD_H_
#define _UTHREAD_H_

#ifdef CONFIG_UTHREAD

int uthread_create(void (*fn)(void *), void *arg, size_t stack_sz);
void uthread_free_all(void);
/* Returns false when all threads are done */
bool uthread_schedule(void);

#else

static inline int uthread_create(void (*fn)(void *), void *arg, size_t stack_sz)
{
	fn(arg);
	return 0;
}

static inline void uthread_free_all(void) { }

static inline bool uthread_schedule(void) { return false; }

#endif /* CONFIG_UTHREAD */
#endif /* _UTHREAD_H_ */
