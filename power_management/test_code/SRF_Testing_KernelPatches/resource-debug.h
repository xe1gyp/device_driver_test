/*
 * linux/arch/arm/plat-omap/include/mach/resource-debug.h
 * OMAP3 SRF test header file
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
 *
 */

#include <linux/module.h>
#include <mach/resource.h>
#include <mach/omap34xx.h>

#define TEST_PASS	0
#define TEST_FAIL	-1

/* No of resources */
#define NO_OF_OPP_RESOURCES		2
#define NO_OF_FREQ_RESOURCES		2
#define NO_OF_PWRDM_LAT_RESOURCES	7
#define NO_OF_CORE_MPU_LAT_RESOURCES	2
#define NO_OF_OPP_FREQ_RESOURCES	(NO_OF_OPP_RESOURCES+	\
						NO_OF_FREQ_RESOURCES)
#define NO_OF_RESOURCES			(NO_OF_OPP_FREQ_RESOURCES + 	    \
						NO_OF_PWRDM_LAT_RESOURCES+  \
						NO_OF_CORE_MPU_LAT_RESOURCES)
/* No of test values to test each resource (atleast 4) */
#define NO_OF_TEST_VALUES		5

/* Convert VDD2 level to throughput */
#define VDD2_LEVEL3_THROUGHPUT		400000
#define VDD2_LEVEL2_THROUGHPUT		200000

/* Power Domain Latency levels */
#define PD_LATENCY_OFF		0x0
#define PD_LATENCY_RET		0x1
#define PD_LATENCY_INACT	0x2
#define PD_LATENCY_ON		0x3

static const char *res_names[] = {
	/* List the resources in this order */
	/* OPP resources */
	"vdd1_opp",
	"vdd2_opp",
	/* frequency resources */
	"mpu_freq",
	"dsp_freq",
	/* Power Domain Latency resources */
	"iva2_pwrdm_latency",
	"sgx_pwrdm_latency",
	"dss_pwrdm_latency",
	"cam_pwrdm_latency",
	"per_pwrdm_latency",
	"neon_pwrdm_latency",
	"usbhost_pwrdm_latency",
	/* MPU/core Latency resources */
	"mpu_latency",
	"core_latency"
};

/* vdd1 test values table - order high to low */
static unsigned long vdd1_opp_test_table[NO_OF_TEST_VALUES] = {
	VDD1_OPP5,
	VDD1_OPP4,
	VDD1_OPP3,
	VDD1_OPP2,
	VDD1_OPP1
};

/* vdd2 test values table - order high to low */
static unsigned long vdd2_opp_test_table[NO_OF_TEST_VALUES] = {
	VDD2_OPP3,
	VDD2_OPP3,
	VDD2_OPP3,
	VDD2_OPP2,
	VDD2_OPP2
};

/* mpu_freq test values table - order high to low */
static unsigned long mpu_freq_test_table[NO_OF_TEST_VALUES] = {
	600000000,
	550000000,
	500000000,
	250000000,
	125000000
};

/* dsp_freq test values table - order high to low */
static unsigned long dsp_freq_test_table[NO_OF_TEST_VALUES] = {
	430000000,
	400000000,
	360000000,
	180000000,
	90000000
};

/* MPU latency test values table - order high to low */
static unsigned long mpu_latency_test_table[NO_OF_TEST_VALUES] = {
	1100,
	800,
	350,
	200,
	10
};

/* Core latency test values table - order high to low */
static unsigned long core_latency_test_table[NO_OF_TEST_VALUES] = {
	1100,
	800,
	350,
	200,
	10
};

/* iva2 powerdomain latency reference values table */
/* pwrdmn latency reference table does not include PD_LATENCY_INACT value */
static unsigned long iva2_pwrdm_ref_table[] = {
	1100,
	350,
	0
};

/* iva2 powerdomain latency test values table - order high to low */
static unsigned long iva2_pwrdm_test_table[NO_OF_TEST_VALUES] = {
	1150,
	800,
	350,
	30,
	10
};

/* sgx powerdomain latency reference values table */
static unsigned long sgx_pwrdm_ref_table[] = {
	1000,
	100,
	0
};

/* sgx powerdomain latency test values table - order high to low */
static unsigned long sgx_pwrdm_test_table[NO_OF_TEST_VALUES] = {
	1100,
	900,
	210,
	90,
	10
};

/* dss powerdomain latency reference values table */
static unsigned long dss_pwrdm_ref_table[] = {
	70,
	20,
	0
};

/* dss powerdomain latency test values table - order high to low */
static unsigned long dss_pwrdm_test_table[NO_OF_TEST_VALUES] = {
	100,
	80,
	55,
	200,
	10
};

/* cam powerdomain latency reference values table */
static unsigned long cam_pwrdm_ref_table[] = {
	850,
	35,
	0
};

/* cam powerdomain latency test values table - order high to low */
static unsigned long cam_pwrdm_test_table[NO_OF_TEST_VALUES] = {
	900,
	800,
	400,
	10,
	5
};

/* per powerdomain latency reference values table */
static unsigned long per_pwrdm_ref_table[] = {
	200,
	110,
	0
};

/* per powerdomain latency test values table - order high to low */
static unsigned long per_pwrdm_test_table[NO_OF_TEST_VALUES] = {
	300,
	190,
	100,
	90,
	5
};

/* neon powerdomain latency reference values table - order high to low */
static unsigned long neon_pwrdm_ref_table[] = {
	200,
	35,
	0
};

/* neon powerdomain latency test values table */
static unsigned long neon_pwrdm_test_table[NO_OF_TEST_VALUES] = {
	300,
	150,
	90,
	10,
	5
};

/* usbhost powerdomain latency reference values table - order high to low */
static unsigned long usbhost_pwrdm_ref_table[] = {
	800,
	150,
	0
};

/* usbhost powerdomain latency test values table - order high to low */
static unsigned long usbhost_pwrdm_test_table[NO_OF_TEST_VALUES] = {
	900,
	200,
	190,
	90,
	5
};

/* Resources test values table array*/
static unsigned long *res_test_table[] = {
	/* List the resources in the same order as res_names*/
	/* OPP resources */
	vdd1_opp_test_table,
	vdd2_opp_test_table,
	/* Frequency resources */
	mpu_freq_test_table,
	dsp_freq_test_table,
	/* Power Domain Latency resources */
	iva2_pwrdm_test_table,
	sgx_pwrdm_test_table,
	dss_pwrdm_test_table,
	cam_pwrdm_test_table,
	per_pwrdm_test_table,
	neon_pwrdm_test_table,
	usbhost_pwrdm_test_table,
	/* Latency resources */
	mpu_latency_test_table,
	core_latency_test_table
};

/* Resources latency reference table array*/
/* pwrdmn latency reference table does not include PD_LATENCY_INACT value */
static unsigned long *lat_ref_table[] = {
	/* List the resources in the same order as res_names*/
	/* OPP resources */
	NULL,
	NULL,
	/* Frequency resources */
	NULL,
	NULL,
	/* Power Domain Latency resources */
	iva2_pwrdm_ref_table,
	sgx_pwrdm_ref_table,
	dss_pwrdm_ref_table,
	cam_pwrdm_ref_table,
	per_pwrdm_ref_table,
	neon_pwrdm_ref_table,
	usbhost_pwrdm_ref_table,
	/* MPU/core Latency resources */
	NULL,
	NULL
};
