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

#endif /* _SETJMP_H_ */
