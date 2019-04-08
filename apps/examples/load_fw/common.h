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
#ifdef VERSION_2_PM_CLIENT
#include <xillibpm_node.h>
#include <xpm_client_api.h>
#define NODEID_DDR0 NODEID(XPM_NODECLASS_DEVICE, XPM_NODESUBCL_DEV_MEM, XPM_NODETYPE_DEV_DDR, XPM_NODEIDX_DEV_DDR_0)
#define NODEID_DDR1 NODEID(XPM_NODECLASS_DEVICE, XPM_NODESUBCL_DEV_MEM, XPM_NODETYPE_DEV_DDR, XPM_NODEIDX_DEV_DDR_1)
#define APU_DEVID(IDX)	NODEID(XPM_NODECLASS_DEVICE, \
						XPM_NODESUBCL_DEV_CORE, \
						XPM_NODETYPE_DEV_CORE_APU, (IDX))
#define APU_NODE_0 XPM_NODEIDX_DEV_ACPU_0
#define APU_NODE_1 XPM_NODEIDX_DEV_ACPU_1
#define APU_POWER_CYCLE_NODE_ID(IDX) APU_DEVID(IDX)
#elif VERSION_1_PM_CLIENT
#include <pm_api_sys.h>
#include <pm_defs.h>
#define APU_NODE_0 NODE_APU_0
#define APU_NODE_1 NODE_APU_3
#define APU_POWER_CYCLE_NODE_ID(IDX) (IDX)
#endif /* VERSION_2_PM_CLIENT */

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
