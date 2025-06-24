# SPDX-License-Identifier: GPL-2.0+
#
# (C) Copyright 2000-2006
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.

extra-y	= start.o
obj-y	= cpu.o cache.o cp15.o

ifdef	CONFIG_XPL_BUILD
ifdef	CONFIG_SPL_NO_CPU_SUPPORT
extra-y	:=
endif
endif

obj-$(if $(filter mxs,$(SOC)),y) += mxs/
obj-$(CONFIG_ARCH_SUNXI) += sunxi/

# some files can only build in ARM or THUMB2, not THUMB1

ifdef CONFIG_$(PHASE_)SYS_THUMB_BUILD
ifndef CONFIG_HAS_THUMB2

AFLAGS_REMOVE_cp15.o := -mthumb -mthumb-interwork

endif
endif
