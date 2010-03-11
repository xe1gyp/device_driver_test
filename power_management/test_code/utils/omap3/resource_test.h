/*
 * SRF/resource-debug-test.h
 * OMAP3 SRF test file
 *
 * Copyright (C) 2009-2010 Texas Instruments, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * History:
 *
 * 2010-02-22: David Miramontes     Initial version of the testcode
 *
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/file.h>
#include <linux/mm.h>

#include <plat/resource.h>

#define RES_INVALID_LEVEL_01	2

/*
 * mpu_latency/core_latency are used to control the cpuidle C state.
 */
void init_latency(struct shared_resource *resp);
int set_latency(struct shared_resource *resp, u32 target_level);

static u8 core_qos_req_added;

static struct shared_resource_ops lat_res_ops = {
	.init		= init_latency,
	.change_level   = set_latency,
};

static struct shared_resource resource_shared_01 = {
	.name		= "TEST_CORE_LATENCY",
	.omap_chip	= OMAP_CHIP_INIT(CHIP_IS_OMAP3430),
	.flags		= RES_INVALID_LEVEL_01,
	.resource_data	= &core_qos_req_added,
	.ops		= &lat_res_ops,
};
