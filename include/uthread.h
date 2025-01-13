/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright 2025 Linaro Limited
 */

#include <linux/types.h>

#ifndef _UTHREAD_H_
#define _UTHREAD_H_

#ifdef CONFIG_UTHREAD

/**
 * uthread_create() - Create a uthread object and make it ready for execution
 *
 * Threads are automatically deleted when they return from their entry point.
 *
 * @fn: the thread's entry point
 * @arg: argument passed to the thread's entry point
 * @stack_sz: stack size for the new thread (in bytes). The stack is allocated
 * on the heap.
 * @grp_id: an optional thread group ID that the new thread should belong to
 * (zero for no group)
 */
int uthread_create(void (*fn)(void *), void *arg, size_t stack_sz,
		   unsigned int grp_id);
/**
 * uthread_schedule() - yield the CPU to the next runnable thread
 *
 * This function is called either by the main thread or any secondary thread
 * (that is, any thread created via uthread_create()) to switch execution to
 * the next runnable thread.
 *
 * Return: true if a thread was scheduled, false if no runnable thread was found
 */
bool uthread_schedule(void);
/**
 * uthread_grp_new_id() - return a new ID for a thread group
 *
 * Return: the new thread group ID
 */
unsigned int uthread_grp_new_id(void);
/**
 * uthread_grp_done() - test if all threads in a group are done
 *
 * @grp_id: the ID of the thread group that should be considered
 * Return: false if the group contains at least one runnable thread (i.e., one
 * thread which entry point has not returned yet), true otherwise
 */
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
