// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

// Copyright 2018 Sen Han <00hnes@gmail.com>
// Copyright 2025 Linaro Limited

#include <coroutines.h>
#include <stdio.h>
#include <stdint.h>


/* Current co-routine */
struct co *current_co;

struct co_stack *co_stack_new(size_t sz)
{
	struct co_stack *p = calloc(1, sizeof(*p));
	uintptr_t u_p;

	if (!p)
		return NULL;

	if (sz < 4096)
		sz = 4096;

	p->sz = sz;
	p->ptr = malloc(sz);
	if (!p->ptr) {
		free(p);
		return NULL;
	}

	p->owner = NULL;
	u_p = (uintptr_t)(p->sz - (sizeof(void*) << 1) + (uintptr_t)p->ptr);
	u_p = (u_p >> 4) << 4;
	p->align_highptr = (void*)u_p;
	p->align_retptr  = (void*)(u_p - sizeof(void*));
	assert(p->sz > (16 + (sizeof(void*) << 1) + sizeof(void*)));
	p->align_limit = p->sz - 16 - (sizeof(void*) << 1);

	return p;
}

void co_stack_destroy(struct co_stack *s){
	if (!s)
		return;
	free(s->ptr);
	free(s);
}

struct co *co_create(struct co *main_co,
		     struct co_stack *stack,
		     size_t save_stack_sz,
		     void (*fp)(void), void *arg)
{
	struct co *p = malloc(sizeof(*p));
	assert(p);
	memset(p, 0, sizeof(*p));

	if (main_co) {
		assert(stack);
		p->stack = stack;
		p->reg[CO_REG_IDX_RETADDR] = (void *)fp;
		// FIXME original code uses align_retptr; causes a crash
		p->reg[CO_REG_IDX_SP] = p->stack->align_highptr;
		p->main_co = main_co;
		p->arg = arg;
		p->fp = fp;
		if (!save_stack_sz)
			save_stack_sz = 64;
		p->save_stack.ptr = malloc(save_stack_sz);
		assert(p->save_stack.ptr);
		p->save_stack.sz = save_stack_sz;
		p->save_stack.valid_sz = 0;
	} else {
		p->main_co = NULL;
		p->arg = arg;
		p->fp = fp;
		p->stack = NULL;
		p->save_stack.ptr = NULL;
	}
	return p;
}

static void grab_stack(struct co *resume_co)
{
	struct co *owner_co = resume_co->stack->owner;

	if (owner_co) {
		assert(owner_co->stack == resume_co->stack);
		assert((uintptr_t)(owner_co->stack->align_retptr) >=
		       (uintptr_t)(owner_co->reg[CO_REG_IDX_SP]));
		assert((uintptr_t)owner_co->stack->align_highptr -
				(uintptr_t)owner_co->stack->align_limit
			<= (uintptr_t)owner_co->reg[CO_REG_IDX_SP]);
		owner_co->save_stack.valid_sz =
			(uintptr_t)owner_co->stack->align_retptr -
			(uintptr_t)owner_co->reg[CO_REG_IDX_SP];
		if (owner_co->save_stack.sz < owner_co->save_stack.valid_sz) {
			free(owner_co->save_stack.ptr);
			owner_co->save_stack.ptr = NULL;
			do {
				owner_co->save_stack.sz <<= 1;
				assert(owner_co->save_stack.sz > 0);
			} while (owner_co->save_stack.sz <
				 owner_co->save_stack.valid_sz);
			owner_co->save_stack.ptr =
				malloc(owner_co->save_stack.sz);
			assert(owner_co->save_stack.ptr);
		}
		if (owner_co->save_stack.valid_sz > 0)
			memcpy(owner_co->save_stack.ptr,
			       owner_co->reg[CO_REG_IDX_SP],
			       owner_co->save_stack.valid_sz);
		if (owner_co->save_stack.valid_sz >
		    owner_co->save_stack.max_cpsz)
			owner_co->save_stack.max_cpsz =
				owner_co->save_stack.valid_sz;
		owner_co->stack->owner = NULL;
		owner_co->stack->align_validsz = 0;
	}
	assert(!resume_co->stack->owner);
	assert(resume_co->save_stack.valid_sz <=
	       resume_co->stack->align_limit - sizeof(void *));
	if (resume_co->save_stack.valid_sz > 0)
		memcpy((void*)
		       (uintptr_t)(resume_co->stack->align_retptr) -
				resume_co->save_stack.valid_sz,
		       resume_co->save_stack.ptr,
		       resume_co->save_stack.valid_sz);
	if (resume_co->save_stack.valid_sz > resume_co->save_stack.max_cpsz)
		resume_co->save_stack.max_cpsz = resume_co->save_stack.valid_sz;
	resume_co->stack->align_validsz =
		resume_co->save_stack.valid_sz + sizeof(void *);
	resume_co->stack->owner = resume_co;
}

void co_resume(struct co *resume_co)
{
	assert(resume_co && resume_co->main_co && !resume_co->done);

	if (resume_co->stack->owner != resume_co)
		grab_stack(resume_co);

	current_co = resume_co;
	_co_switch(resume_co->main_co, resume_co);
	current_co = resume_co->main_co;
}

void co_destroy(struct co *co){
	if (!co)
		return;

	if(co_is_main_co(co)){
		free(co);
		current_co = NULL;
	} else {
		if(co->stack->owner == co){
			co->stack->owner = NULL;
			co->stack->align_validsz = 0;
		}
		free(co->save_stack.ptr);
		co->save_stack.ptr = NULL;
		free(co);
	}
}
