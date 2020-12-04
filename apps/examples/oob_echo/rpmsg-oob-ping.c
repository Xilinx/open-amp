/* This is a sample demonstration application that showcases usage of rpmsg 
This application is meant to run on the remote CPU running baremetal code. 
This application echoes back data that was sent to it by the master core. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <openamp/open_amp.h>
#include <metal/alloc.h>
#include "platform_info.h"
#include "rpmsg-echo.h"
#include <metal/io.h>
#include <metal/device.h>

#define APP_EPT_ADDR    0
#define LPRINTF(format, ...) printf(format, ##__VA_ARGS__)
#define LPERROR(format, ...) LPRINTF("ERROR: " format, ##__VA_ARGS__)

#define NUM_BUFFERS 16
#define NUM_MESSAGES_TO_SEND 32

#define DEV_BUS_NAME "platform"

static void* large_buffer;

static char data_to_send[BUFFER_SIZE];
static const char* message_string = "Out of Band Message contents : ";
/* Globals */
static struct rpmsg_endpoint lept;
static int ept_deleted = 0;

static struct metal_device *large_buffer_shm_device;
static struct metal_io_region * large_buffer_io;
static int msg_received = 0;

static int setup_buffer(struct packet * p, unsigned buffer_index, unsigned packet_length, void * data){
	int ret;

	if (buffer_index > NUM_BUFFERS || packet_length > BUFFER_SIZE || !data)
		LPERROR("send_buffer failed\r\n");
	p->packet_type = OUT_OF_BAND | DATA_MSG;
	p->buffer_index = buffer_index;
	p->packet_length = packet_length;
	ret = metal_io_block_write(large_buffer_io, (BUFFER_SIZE * buffer_index), data, packet_length);
	if (ret < 0){
		LPERROR("Unable to metal_io_block_write()\n");
		return -1;
	}
	LPRINTF("APU copied to large buffer \r\n");

	return 0;
}


/*-----------------------------------------------------------------------------*
 *  RPMSG endpoint callbacks
 *-----------------------------------------------------------------------------*/
static int rpmsg_endpoint_cb(struct rpmsg_endpoint *ept, void *data, size_t len,
			     uint32_t src, void *priv)
{
	(void)priv;
	(void)src;
	(void)len;
	(void)ept;

	struct packet * p = (struct packet *)data;

	LPRINTF("APU  message is received.\r\n" );
	msg_received = 1;
	LPRINTF("APU message contents : packet_type %x buffer_index %x packet_length %x\r\n", 
		p->packet_type, p->buffer_index, p->packet_length );
	if (p->packet_type & ACK_MSG){
		LPRINTF("APU received ACK_MSG");
	}
	return RPMSG_SUCCESS;
}

static void rpmsg_service_unbind(struct rpmsg_endpoint *ept)
{
	(void)ept;
	rpmsg_destroy_ept(&lept);
	LPRINTF("echo test: service is destroyed\r\n");
	ept_deleted = 1;
}

static void rpmsg_name_service_bind_cb(struct rpmsg_device *rdev,
				       const char *name, uint32_t dest)
{
	LPRINTF("new endpoint notification is received.\r\n");
	if (strcmp(name, RPMSG_SERVICE_NAME))
		LPERROR("Unexpected name service %s.\r\n", name);
	else
		(void)rpmsg_create_ept(&lept, rdev, RPMSG_SERVICE_NAME,
				       APP_EPT_ADDR, dest,
				       rpmsg_endpoint_cb,
				       rpmsg_service_unbind);

}

/*-----------------------------------------------------------------------------*
 *  Application
 *-----------------------------------------------------------------------------*/
int app (struct rpmsg_device *rdev, void *priv)
{
	int ret;
	/* Create RPMsg endpoint */
	ret = rpmsg_create_ept(&lept, rdev, RPMSG_SERVICE_NAME, APP_EPT_ADDR,
				   RPMSG_ADDR_ANY,
				   rpmsg_endpoint_cb, rpmsg_service_unbind);

	if (ret) {
		LPERROR("Failed to create RPMsg endpoint.\r\n");
		return ret;
	}
	while (!is_rpmsg_ept_ready(&lept))
		platform_poll(priv);

	struct packet * p;
	p = (struct packet *)metal_allocate_memory(sizeof(struct packet));
	if (!p){
		LPERROR("memory allocation failed for packet.\r\n");
		return -1;
	}

	large_buffer = metal_allocate_memory(sizeof(BUFFER_SIZE*sizeof(char)));
	if (!large_buffer){
		LPERROR("memory allocation failed for packet.\r\n");
		return -1;
	}

	LPRINTF("APU begin demo \r\n");
	for(int number_messages_sent = 0; number_messages_sent < NUM_MESSAGES_TO_SEND;  number_messages_sent++) {
		msg_received = 0;
		strcpy(data_to_send, message_string);
		LPRINTF("APU: contents of message %s \r\n", data_to_send);

		ret = setup_buffer(p, number_messages_sent % NUM_BUFFERS, sizeof(data_to_send), data_to_send);
		if (ret){
			LPERROR("setup_buffer failed \r\n");
			return RPMSG_ERR_PARAM;
		}

		if (rpmsg_send(&lept, p, sizeof(struct packet)) < 0){ 
			LPERROR("rpmsg_send failed\r\n");
			return RPMSG_ERR_PARAM;
		}
		do {
			platform_poll(priv);
		} while (number_messages_sent < NUM_MESSAGES_TO_SEND && !ept_deleted && !msg_received);

	}
	LPRINTF("APU side ending demo \r\n");
	metal_free_memory(p);

	rpmsg_destroy_ept(&lept);

	return 0;
}

int main(int argc, char *argv[])
{
	void *platform;
	struct rpmsg_device *rpdev;
	int ret;

	ret = platform_init(argc, argv, &platform);
	if (ret) {
		LPERROR("Failed to initialize platform.\r\n");
		ret = -1;
	} else {
		/* Initialize platform */
		printf("try to open device \r\n");
		ret = metal_device_open(DEV_BUS_NAME, "3ee20000.shm", &large_buffer_shm_device);
		if (ret) {
			fprintf(stderr, "ERROR: failed to open large_buffer_shm_device  device: %d.\r\n", ret);
			return -1;
		}

		printf("able to open device. now try to get io region from device \r\n");
		printf("large_buffer_shm_device->num_regions %u \r\n", large_buffer_shm_device->num_regions);
		large_buffer_io = metal_device_io_region(large_buffer_shm_device, 0);
		if (!large_buffer_io){
			fprintf(stderr, "ERROR: failed to open large_buffer_io \r\n");
			return -1;
		}

		printf("able to get io region from device \r\n");

		rpdev = platform_create_rpmsg_vdev(platform, 0,
						  VIRTIO_DEV_MASTER,
						  NULL,
						  rpmsg_name_service_bind_cb);
		if (!rpdev) {
			LPERROR("Failed to create rpmsg virtio device.\r\n");
			ret = -1;
		} else {
			app(rpdev, platform);
			platform_release_rpmsg_vdev(rpdev, platform);
			ret = 0;
		}
	}

	LPRINTF("Stopping application...\r\n");
	platform_cleanup(platform);

	return ret;
}

