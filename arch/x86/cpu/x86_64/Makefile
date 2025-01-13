#
# (C) Copyright 2016 Google, Inc
# Written by Simon Glass <sjg@chromium.org>
#

obj-y += cpu.o interrupts.o setjmp.o

ifndef CONFIG_EFI
obj-y += misc.o
endif

obj-$(CONFIG_COUROUTINES) += co_switch.o
