/*
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <stdio.h>
#include <openamp/open_amp.h>
#include <openamp/version.h>
#include <metal/alloc.h>
#include <metal/log.h>
#include <metal/version.h>
#include "platform_info.h"
#include "rpmsg-echo.h"
#include "FreeRTOS.h"
#include "task.h"

#define LPRINTF(format, ...) xil_printf(format, ##__VA_ARGS__)
#define LPERROR(fmt, ...) LPRINTF("ERROR: " fmt, ##__VA_ARGS__)

static TaskHandle_t comm_task;

/* RPMsg Task */
static void rpmsg_listen_task(void *unused_arg)
{
	void *platform;
	struct rpmsg_device *rpdev;

	(void)(int *)unused_arg;

	/* can't use ML_INFO, metal_log setup is in init_system */
	LPRINTF("openamp lib version: %s (", openamp_version());
	LPRINTF("Major: %d, ", openamp_version_major());
	LPRINTF("Minor: %d, ", openamp_version_minor());
	LPRINTF("Patch: %d)\r\n", openamp_version_patch());

	LPRINTF("libmetal lib version: %s (", metal_ver());
	LPRINTF("Major: %d, ", metal_ver_major());
	LPRINTF("Minor: %d, ", metal_ver_minor());
	LPRINTF("Patch: %d)\r\n", metal_ver_patch());

	LPRINTF("Starting application...\r\n");

	/* Initialize platform */
	if (platform_init(0, NULL, &platform)) {
		LPERROR("Failed to initialize platform.\r\n");
	} else {
		rpdev = platform_create_rpmsg_vdev(platform, 0,
						   VIRTIO_DEV_DEVICE,
						   NULL, NULL);
		if (!rpdev) {
			ML_ERR("Failed to create rpmsg virtio device.\r\n");
		} else {
			app(rpdev, platform);
			platform_release_rpmsg_vdev(rpdev, platform);
		}
	}

	ML_INFO("Stopping application...\r\n");
	platform_cleanup(platform);

	/* Terminate this task */
	vTaskDelete(NULL);
}

/* Application entry point */
int main(void)
{
	BaseType_t stat;

	/* Create the tasks */
	stat = xTaskCreate(rpmsg_listen_task, (const char *)"RPMsg task", 1024, NULL, 2,
			   &comm_task);
	if (stat != pdPASS) {
		LPERROR("cannot create task\r\n");
		while (1);
	}

	/* Start running FreeRTOS tasks */
	vTaskStartScheduler();

	/* Will not get here, unless a call is made to vTaskEndScheduler() */
	while (1);

	/* suppress compilation warnings*/
	return 0;
}
