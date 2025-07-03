/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * U-Boot - linkage.h
 *
 * Copyright (c) 2005-2007 Analog Devices Inc.
 */

#ifndef _LINUX_LINKAGE_H
#define _LINUX_LINKAGE_H

#include <asm/linkage.h>

/* Some toolchains use other characters (e.g. '`') to mark new line in macro */
#ifndef ASM_NL
#define ASM_NL		 ;
#endif

#ifdef __cplusplus
#define CPP_ASMLINKAGE		extern "C"
#else
#define CPP_ASMLINKAGE
#endif

#ifndef asmlinkage
#define asmlinkage CPP_ASMLINKAGE
#endif

#define SYMBOL_NAME_STR(X)	#X
#define SYMBOL_NAME(X)		X
#ifdef __STDC__
#define SYMBOL_NAME_LABEL(X)	X##:
#else
#define SYMBOL_NAME_LABEL(X)	X:
#endif

#ifndef __ALIGN
#define __ALIGN .align		4
#endif

#ifndef __ALIGN_STR
#define __ALIGN_STR		".align 4"
#endif

#ifdef __ASSEMBLY__

#define ALIGN			__ALIGN
#define ALIGN_STR		__ALIGN_STR

#define LENTRY(name) \
	ALIGN ASM_NL \
	SYMBOL_NAME_LABEL(name)

#define ENTRY2(name) \
	.globl SYMBOL_NAME(name) ASM_NL \
	LENTRY(name)

#define WEAK2(name) \
	.weak SYMBOL_NAME(name) ASM_NL \
	LENTRY(name)

#define ENTRY(name) \
	.pushsection .text.##name,"ax",%progbits ASM_NL \
	ENTRY2(name)

#define WEAK(name) \
	.pushsection .text.##name,"ax",%progbits ASM_NL \
	WEAK2(name)

#ifndef END
#define END(name) \
	.size name, .-name
#endif

#define ENDPROC2(name) \
	.type name STT_FUNC ASM_NL \
	END(name)
#endif

#ifndef ENDPROC
#define ENDPROC(name) \
	ENDPROC2(name) ASM_NL \
	.popsection

#endif

#endif
