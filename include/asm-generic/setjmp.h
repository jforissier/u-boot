/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright 2025 Linaro Ltd.
 */

#ifndef _SETJMP_H_
#define _SETJMP_H_

#include <asm/setjmp_bits.h>

/*
 * This really should be opaque, but the EFI implementation wrongly
 * assumes that a 'struct jmp_buf_data' is defined.
 */
typedef struct jmp_buf_data jmp_buf[1];

int setjmp(jmp_buf jmp);
__noreturn void longjmp(jmp_buf jmp, int ret);

#ifdef CONFIG_HAVE_INITJMP
/**
 * initjmp - initialize jmp_buf to branch to a given function with a given stack
 *
 * This function sets up a jump buffer for later use with longjmp(). In the
 * traditional setjmp()/longjmp() pair, longjmp() branches immediately after the
 * setjmp() which appears to return a non-zero status to indicate that the long
 * jump has occured. The stack is restored as it was saved in the jump buffer by
 * setjmp(). This initjmp() function however allows to use longjmp() to branch
 * to any function and with a specific stack. Please note that @func MUST NOT
 * return. It shall typically restore the main stack and resume execution by
 * simply doing a longjmp() to a jump buffer set in the main thread before the
 * longjmp() call. initjmp() allows to implement multithreading.
 *
 * @jmp: jump buffer to initialize
 * @func: function to be called on longjm(), MUST NOT RETURN
 * @stack_top: the stack to be used by @func
 */
int initjmp(jmp_buf jmp, void __noreturn (*func)(void), void *stack_top);
#endif

#endif /* _SETJMP_H_ */
