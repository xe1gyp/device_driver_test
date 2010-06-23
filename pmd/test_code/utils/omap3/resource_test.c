/*
 * SRF/resource-debug-test.c
 * OMAP3 SRF test file
 *
 * Copyright (C) 2009-2010 Texas Instruments, Inc.
 * Charulatha Varadarajan <charu@ti.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 * History:
 *
 * 2009-09-17: Charulatha V         Initial code
 * 2010-02-22: David Miramontes     Adding support to change opp levels
 *
 */

#include <linux/pm_qos_params.h>

#include "resource_test.h"

#define MAX_USERS	10
struct  users_list usr_list[MAX_USERS];

/**
 * init_latency - Initializes the mpu/core latency resource.
 * @resp: Latency resource to be initalized
 *
 * No return value.
 */
void init_latency(struct shared_resource *resp)
{
	resp->no_of_users = 0;
	resp->curr_level = RES_LATENCY_DEFAULTLEVEL;
	*((u8 *)resp->resource_data) = 0;
	return;
}

/**
 * set_latency-Adds/Updates and removes the CPU_DMA_LATENCY in *pm_qos_params
 * @resp: resource pointer
 * @latency: target latency to be set
 *
 * Returns 0 on success, or error values as returned by
 * pm_qos_update_requirement/pm_qos_add_requirement.
 */
int set_latency(struct shared_resource *resp, u32 latency)
{
	u8 *pm_qos_req_added;

	if (resp->curr_level == latency)
		return 0;
	else {
		printk("\nUpdate the resources current level\n");
		resp->curr_level = latency;
	}

	pm_qos_req_added = resp->resource_data;

	if (latency == RES_LATENCY_DEFAULTLEVEL)
		/* No more users left, remove the pm_qos_req if present */
		if (*pm_qos_req_added) {
			pm_qos_remove_requirement(PM_QOS_CPU_DMA_LATENCY,
							resp->name);
			*pm_qos_req_added = 0;
			return 0;
		}

	if (*pm_qos_req_added) {
		return pm_qos_update_requirement(PM_QOS_CPU_DMA_LATENCY,
						resp->name, latency);
	} else {
		printk("\nInserting a named element in the list "
			"for that identified PM_QOS parameter "
			"with the target value.\n");
		*pm_qos_req_added = 1;
		return pm_qos_add_requirement(PM_QOS_CPU_DMA_LATENCY,
						resp->name, latency);
	}

}

/**
 * update_resource_level - Regenerates and updates the curr_level of the res
 * @resp: Pointer to the resource
 *
 * This function looks at all the users of the given resource and the levels
 * requested by each of them, and recomputes a target level for the resource
 * acceptable to all its current usres. It then calls platform specific
 * change_level to change the level of the resource.
 * Returns 0 on success, else a non-zero value returned by the platform
 * specific change_level function.
 **/
static int update_resource_level(struct shared_resource *resp)
{
	unsigned long target_level;
	struct users_list *user;
	int ret;

	/* Regenerate the target_value for the resource */
	target_level = RES_INVALID_LEVEL_01;
	printk("Using Invalid Level. \n");

	printk("SRF: Changing Level for resource %s to %ld (%s)\n",
		resp->name,
		target_level,
		(target_level == 0 ? "PERFORMANCE_LEVEL" :
			(target_level == -1 ? "LATENCY_LEVEL" :
				"Invalid Level")));

	ret = resp->ops->change_level(resp, target_level);
	if (ret) {
		printk(KERN_ERR "Unable to Change"
					"level for resource %s to %ld\n",
		resp->name, target_level);
	}
	return ret;
}

static int resource_test(void)
{
	int error;
	error = update_resource_level(&resource_shared_01);
	if (error)
		return 1;
	printk("\nDone\n");
	return 0;
}

int __init resource_test_init(void)
{
	printk("\nInit of Resource Test...\n");
	return resource_test();
}

void __exit resource_test_exit(void)
{
	printk(" \nExiting Module \n");
}

module_init(resource_test_init);
module_exit(resource_test_exit);

MODULE_AUTHOR("Texas Instruments");
MODULE_LICENSE("GPL");
