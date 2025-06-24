/* SPDX-License-Identifier: GPL-2.0+ */

#include <linux/linkage.h>

ENTRY(readfr_extra_feature_reg)
	mrc	p15, 1, r0, c15, c1, 0 @ readfr exfr
	bx	lr
ENDPROC(readfr_extra_feature_reg)

ENTRY(_writefr_extra_feature_reg)
	mcr	p15, 1, r0, c15, c1, 0 @ writefr exfr
	bx	lr
ENDPROC(_writefr_extra_feature_reg)
