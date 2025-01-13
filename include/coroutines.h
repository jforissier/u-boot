/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/*
 * Copyright 2018 Sen Han <00hnes@gmail.com>
 * Copyright 2025 Linaro Limited
 */

#ifndef _COROUTINES_H_
#define _COROUTINES_H_

#ifndef CONFIG_COROUTINES

static inline void co_yield(void) {}
static inline void co_exit(void) {}

#else

#ifdef __UBOOT__
#include <log.h>
#else
#include <assert.h>
#endif
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef __aarch64__
#define CO_REG_IDX_RETADDR 0
#define CO_REG_IDX_SP 1
#else
#error Architecture no supported
#endif

struct co_save_stack {
    void*  ptr;
    size_t sz;
    size_t valid_sz;
    size_t max_cpsz; /* max copy size in bytes */
};

struct co_stack {
    void *ptr;
    size_t sz;
    void *align_highptr;
    void *align_retptr;
    size_t align_validsz;
    size_t align_limit;
    struct co *owner;
    void *real_ptr;
    size_t real_sz;
};

struct co {
	/* CPU state: callee-saved registers plus SP and PC */
	void *reg[14];  // pc, sp, x19-x29, x30 (lr)

	struct co *main_co;
	void *arg;
	bool done;

	void (*fp)(void);

	struct co_save_stack save_stack;
	struct co_stack *stack;
};

extern struct co *current_co;

static inline struct co *co_get_co(void)
{
	return current_co;
}

static inline void *co_get_arg(void)
{
	return co_get_co()->arg;
}

struct co_stack *co_stack_new(size_t sz);

void co_stack_destroy(struct co_stack *s);

struct co *co_create(struct co *main_co,
		     struct co_stack *stack,
		     size_t save_stack_sz, void (*fp)(void),
		     void *arg);

void co_resume(struct co *resume_co);

void co_destroy(struct co *co);

void *_co_switch(struct co *from_co, struct co *to_co);

static inline void _co_yield_to_main_co(struct co *yield_co)
{
    assert(yield_co);
    assert(yield_co->main_co);
    _co_switch(yield_co, yield_co->main_co);
}

static inline void co_yield(void)
{
	if (current_co)
		_co_yield_to_main_co(current_co);
}

static inline bool co_is_main_co(struct co *co)
{
	return !co->main_co;
}

static inline void co_exit(void)
{
	struct co *co = co_get_co();

	if (!co)
		return;
	co->done = true;
	assert(co->stack->owner == co);
	co->stack->owner = NULL;
	co->stack->align_validsz = 0;
	_co_yield_to_main_co(co);
	assert(false);
}

#endif /* CONFIG_COROUTINES */
#endif /* _COROUTINES_H_ */
