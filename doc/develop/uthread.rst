.. SPDX-License-Identifier: GPL-2.0+
.. (C) Copyright 2025 Linaro Limited

Uthread Framework
=================

Introduction
------------

The uthread framework is a basic task scheduler that allows to run functions
"in parallel" on a single CPU core. The scheduling is cooperative, not
preemptive -- meaning that context switches from one task to another task is
voluntary, via a call to uthread_schedule(). This characteristic makes thread
synchronization much easier, because a thread cannot be interrupted in the
middle of a critical section (reading from or writing to shared state, for
instance).

`CONFIG_UTHREAD` in lib/Kconfig enables the uthread framework. When disabled,
the uthread_create()  and uthread_schedule() functions may still be used so
that code differences between uthreads enabled and disabled can be reduced to
a minimum. See details below.

Function description
--------------------

See `lib/uthread.c`.

Usage
-----

This section shows how uthreads may be used to convert sequential code
into parallel code. Error handling is omitted for brevity.
Consider the following:

.. code-block:: C

   static void init_foo(void)
   {
        start_foo();
        while (!foo_is_ready())
            udelay(10);
   }

   static void init_bar(void)
   {
        start_bar();
        while (!bar_is_ready())
            udelay(10);
   }

   void init_foo_bar(void)
   {
        init_foo();
        init_bar();
   }

This example is a simplified version of typical device initialization, where
some commands are sent to a device and the CPU needs to wait for the device
to reply or change state after wich the device is known to be ready.
Assuming devices 'foo' and 'bar' are independant, and assuming they both take
some significant amount of time to initialize, then the above code is clearly
suboptimal because device 'bar' is started only after 'foo' is ready, although
it could have been started at the same time. Therefore a better version would
be:

.. code-block:: C

   void init_foo_bar(void)
   {
        start_foo();
        start_bar();
        while (!foo_is_ready() || !bar_is_ready())
            udelay(10);
   }


Unfortunately, refactoring the code like that is rarely so easy because
init_foo() and init_bar() would in reality involve dozens of functions
and result in deep call stacks. This is where uthreads are helpful. Here is
how.

.. code-block:: C

   /* Unchanged */
   static void init_foo(void)
   {
        start_foo();
        while (!foo_is_ready())
            udelay(10);
   }

   /* Unchanged */
   static void init_bar(void)
   {
        start_bar();
        while (!bar_is_ready())
            udelay(10);
   }

   /* Added only because init_foo() does not take a (void *) */
   static void do_init_foo(void *arg)
   {
        init_foo();
   }

   /* Added only because init_bar() does not take a (void *) */
   static void do_init_bar(void *arg)
   {
        init_bar();
   }

   void init_foo_bar(void)
   {
       int id;

       /* Allocate a thread group ID (optional) */
       id = uthread_grp_new_id();
       /* Create and start two threads */
       uthread_create(do_init_foo, NULL, 0, id);
       uthread_create(do_init_bar, NULL, 0, id);
       /* Wait until both threads are done */
       while (!uthread_grp_done(id))
           uthread_schedule();
   }

When `CONFIG_UTHREAD` is enabled, do_init_foo() is started and quickly yields
the CPU back to the main thread due to udelay() calling uthread_schedule().
Then do_init_bar() is started and it also calls udelay(), which in turn calls
uthread_schedule(). With the main thread entering the scheduling loop, we
effectively have three tasks scheduled in a round-robin fashion until
do_init_foo() and do_init_bar() are both done.

when `CONFIG_UTHREAD` is disabled, uthread_grp_new_id() always returns 0,
uthread_create() simply calls its first argument, uthread_grp_done() always
returns true and uthread_schedule() does nothing. In this case, the code is
functionally equivalent to the sequential version.
