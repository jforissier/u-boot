/* SPDX-License-Identifier: GPL-2.0 */

#include <net/ulwip.h>
#include "lwip/ip_addr.h"

void ping_tmo(void *arg);

#define LWIP_DEBUG 1 /* ping_time is under ifdef*/
#define PING_RESULT(cond) { \
	if (cond == 1) { \
		printf("host %s is alive\n", ipaddr_ntoa(addr)); \
		printf(" %"U32_F" ms\n", (sys_now() - ping_time)); \
		ping_stop(); \
		sys_untimeout(ping_tmo, NULL); \
		sys_check_timeouts(); \
		ulwip_exit(0); \
	} else { \
		printf("ping failed; host %s in not alive\n",\
		       ipaddr_ntoa(addr)); \
		ping_stop(); \
		sys_untimeout(ping_tmo, NULL); \
		sys_check_timeouts(); \
		ulwip_exit(-1); \
	} \
	} while (0);

void ping_init(const ip_addr_t *ping_addr);
void ping_stop(void);
