/*
 * This #include series allows to build ping.c which is in the lwIP contrib
 * directory (unmodified) with the ping.h header from this directory.
 */
#include "lwip/opt.h"
#include "ping.h"
#include "../../lwip-external/contrib/apps/ping/ping.c"
