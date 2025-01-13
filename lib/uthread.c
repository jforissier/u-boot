// SPDX-License-Identifier: GPL-2.0-only
//
// Copyright (C) 2021 Ahmad Fatoum, Pengutronix
// Copyright (C) 2025 Linaro Limited
//
// An implementation of cooperative multi-tasking inspired from barebox threads
// https://github.com/barebox/barebox/blob/master/common/bthread.c

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
	struct list_head list;
} main_thread = {
	.list = LIST_HEAD_INIT(main_thread.list),
};

static struct uthread *current = &main_thread;

static void __noreturn uthread_trampoline(void)
{
	current->fn(current->arg);
	current->done = true;
	current = &main_thread;
	longjmp(current->ctx, 1);
	/* Not reached */
	while (true)
		;
}

static void uthread_free(struct uthread *uthread)
{
	if (!uthread)
		return;
	free(uthread->stack);
	free(uthread);
}

int uthread_create(void (*fn)(void *), void *arg, size_t stack_sz)
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

	list_add_tail(&uthread->list, &current->list);

	initjmp(uthread->ctx, uthread_trampoline, uthread->stack + stack_sz);

	return 0;
err:
	uthread_free(uthread);
	return -1;
}

void uthread_free_all(void)
{
	struct uthread *next;
	struct uthread *tmp;

	list_for_each_entry_safe(next, tmp, &current->list, list) {
		list_del(&next->list);
		uthread_free(next);
	}
}

static void uthread_resume(struct uthread *uthread)
{
	if (!setjmp(current->ctx)) {
		current = uthread;
		longjmp(uthread->ctx, 1);
	}
}

bool uthread_schedule(void)
{
	struct uthread *next;

	list_for_each_entry(next, &current->list, list) {
		if (!next->done) {
			uthread_resume(next);
			return true;
		}
	}
	return false;
}
