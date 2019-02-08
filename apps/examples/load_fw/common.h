/*
 * Copyright(c) 2019 Xilinx Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <metal/alloc.h>
#include <metal/io.h>
#include <metal/sys.h>
#include <metal/utilities.h>
#include <openamp/remoteproc.h>
#include <openamp/remoteproc_loader.h>
#include <stdarg.h>
#include <stdio.h>
/* Xilinx headers */
#include <pm_api_sys.h>
#include <pm_defs.h>
#include <xil_mpu.h>
#include <xil_printf.h>
#include <xreg_cortexr5.h>

#define LPRINTF(format, ...) xil_printf(format, ##__VA_ARGS__)
//#define LPRINTF(format, ...)
#define LPERROR(format, ...) LPRINTF("ERROR: " format, ##__VA_ARGS__)

struct rproc_priv {
    struct remoteproc *rproc;
    int cpu_id;
};

#endif /* COMMON_H_ */
