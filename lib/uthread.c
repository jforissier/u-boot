// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2021 Ahmad Fatoum, Pengutronix
 * Copyright (C) 2025 Linaro Limited
 *
 * An implementation of cooperative multi-tasking inspired from barebox threads
 * https://github.com/barebox/barebox/blob/master/common/bthread.c
 */

#include <compiler.h>
#include <asm/setjmp.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <malloc.h>
#include <stdint.h>
#include <uthread.h>

static struct uthread {
	void (*fn)(void *);
	void *arg;
	jmp_buf ctx;
	void *stack;
	bool done;
	unsigned int grp_id;
	struct list_head list;
} main_thread = {
	.list = LIST_HEAD_INIT(main_thread.list),
};

static struct uthread *current = &main_thread;

/**
 * uthread_trampoline() - Call the current thread's entry point then resume the
 * main thread.
 *
 * This is a helper function which is used as the @func argument to the inijmp()
 * function, and ultimately invoked via setjmp(). It does not return, but
 * instead longjmp()'s back to the main thread.
 */
static void __noreturn uthread_trampoline(void)
{
	struct uthread *curr = current;

	curr->fn(curr->arg);
	curr->done = true;
	current = &main_thread;
	longjmp(current->ctx, 1);
	/* Not reached */
	while (true)
		;
}

/**
 * uthread_free() - Free memory used by a uthread object.
 */
static void uthread_free(struct uthread *uthread)
{
	if (!uthread)
		return;
	free(uthread->stack);
	free(uthread);
}

/**
 * uthread_create() - Create a uthread object and make it ready for execution
 *
 * Threads are automatically deleted when then return from their entry point.
 *
 * @fn: the thread's entry point
 * @arg: argument passed to the thread's entry point
 * @stack_sz: stack size for the new thread (in bytes). The stack is allocated
 * on the heap.
 * @grp_id: an optional thread group ID that the new thread should belong to
 * (zero for no group)
 */
int uthread_create(void (*fn)(void *), void *arg, size_t stack_sz,
		   unsigned int grp_id)
{
	struct uthread *uthread;

	if (!stack_sz)
		stack_sz = CONFIG_UTHREAD_STACK_SIZE;

	uthread = calloc(1, sizeof(*uthread));
	if (!uthread)
		return -1;

	uthread->stack = memalign(16, stack_sz);
	if (!uthread->stack)
		goto err;

	uthread->fn = fn;
	uthread->arg = arg;
	uthread->grp_id = grp_id;

	list_add_tail(&uthread->list, &current->list);

	initjmp(uthread->ctx, uthread_trampoline, uthread->stack + stack_sz);

	return 0;
err:
	uthread_free(uthread);
	return -1;
}

/**
 * uthread_resume() - switch execution to a given thread
 *
 * @uthread: the thread object that should be resumed
 */
static void uthread_resume(struct uthread *uthread)
{
	if (!setjmp(current->ctx)) {
		current = uthread;
		longjmp(uthread->ctx, 1);
	}
}

/**
 * uthread_schedule() - yield the CPU to the next runnable thread
 *
 * This function is called either by the main thread or any secondary thread
 * (that is, any thread created via uthread_create()) to switch execution to
 * the next runnable thread.
 *
 * Return: true if a thread was scheduled, false if no runnable thread was found
 */
bool uthread_schedule(void)
{
	struct uthread *next;
	struct uthread *tmp;

	if (list_empty(&current->list))
	    return false;

	list_for_each_entry_safe(next, tmp, &current->list, list) {
		if (!next->done) {
			uthread_resume(next);
			return true;
		} else {
			/* Found a 'done' thread, free its resources */
			list_del(&next->list);
			uthread_free(next);
		}
	}
	return false;
}

/**
 * uthread_grp_new_id() - return a new ID for a thread group
 *
 * Return: the new thread group ID
 */
unsigned int uthread_grp_new_id(void)
{
	static unsigned int id = 0;

	return ++id;
}

/**
 * uthread_grp_done() - test if all threads in a group are done
 *
 * @grp: the ID of the thread group that should be considered
 * Return: false if the group contains at least one runnable thread (i.e., one
 * thread which entry point has not returned yet), true otherwise
 */
bool uthread_grp_done(unsigned int grp_id)
{
	struct uthread *next;

	list_for_each_entry(next, &main_thread.list, list) {
		if (next->grp_id == grp_id && !next->done)
			return false;
	}

	return true;
}
