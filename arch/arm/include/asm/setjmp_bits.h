/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * (C) Copyright 2017 Theobroma Systems Design und Consulting GmbH
 * (C) Copyright 2016 Alexander Graf <agraf@suse.de>
 */

#ifndef _SETJMP_BITS_H_
#define _SETJMP_BITS_H_

struct jmp_buf_data {
#if defined(__aarch64__)
	u64  regs[13];
#else
	u32  regs[10];  /* r4-r9, sl, fp, sp, lr */
#endif
};

#endif /* _SETJMP_BITS_H_ */
