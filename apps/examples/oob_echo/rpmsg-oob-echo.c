/* This is a sample demonstration application that showcases usage of rpmsg
This application is meant to run on the remote CPU running baremetal code.
This application echoes back data that was sent to it by the master core. */

#include <stdio.h>
#include <openamp/open_amp.h>
#include <metal/alloc.h>
#include "platform_info.h"
#include "rpmsg-echo.h"

#define OUT_OF_BAND (0x1UL<<31)
#define INIT_MSG 0x2UL
#define ACK_MSG 0X3UL
#define DATA_MSG 0x4UL
#define SHUTDOWN_MSG 0x5UL

#define TABLE_BASE_ADDRESS 0x3ee20000UL
#define BUFFER_SIZE 0x10000UL //64K.. can change to 65K as needed
#define NUM_BUFFERS 16
#define NUM_MESSAGES_TO_SEND 32

static struct packet {
	unsigned packet_type;
	unsigned buffer_index;
	unsigned packet_length;
};


#define LPRINTF(format, ...) printf(format, ##__VA_ARGS__)
//#define LPRINTF(format, ...)
#define LPERROR(format, ...) LPRINTF("ERROR: " format, ##__VA_ARGS__)

static struct rpmsg_endpoint lept;
static int shutdown_req = 0;

/*-----------------------------------------------------------------------------*
 *  RPMSG endpoint callbacks
 *-----------------------------------------------------------------------------*/
static int rpmsg_endpoint_cb(struct rpmsg_endpoint *ept, void *data, size_t len,
			     uint32_t src, void *priv)
{
	struct packet * p = (struct packet *)data;
	struct packet ack_packet;

	(void)priv;
	(void)src;
	(void)len;
	LPRINTF("RPU: message is received.\r\n" );
	LPRINTF("RPU: message contents : packet_type %x buffer_index %x packet_length %x\r\n",
	p->packet_type, p->buffer_index, p->packet_length );

	LPRINTF("RPU: Data location at %x \r\n", (unsigned)(TABLE_BASE_ADDRESS+ (BUFFER_SIZE * p->buffer_index)));
	LPRINTF("RPU: contents of message %s \r\n", (char*)((TABLE_BASE_ADDRESS+ (BUFFER_SIZE * p->buffer_index))));
	/* notify remote that message is received */
	ack_packet.packet_type = OUT_OF_BAND | ACK_MSG;
	if (rpmsg_send(ept, &ack_packet, sizeof(struct packet)) < 0){
		LPERROR("RPU rpmsg_send failed\r\n");
		return RPMSG_ERR_PARAM;
	} else
		LPRINTF("RPU: sent ack to APU \r\n");

	return RPMSG_SUCCESS;
}

static void rpmsg_service_unbind(struct rpmsg_endpoint *ept)
{
	(void)ept;
	LPRINTF("RPU unexpected Remote endpoint destroy\r\n");
	shutdown_req = 1;
}

/*-----------------------------------------------------------------------------*
 *  Application
 *-----------------------------------------------------------------------------*/
int app(struct rpmsg_device *rdev, void *priv)
{
	int ret;

	/* Initialize RPMSG framework */
	LPRINTF("Try to create rpmsg endpoint.\r\n");

	ret = rpmsg_create_ept(&lept, rdev, RPMSG_SERVICE_NAME,
			       0, RPMSG_ADDR_ANY, rpmsg_endpoint_cb,
			       rpmsg_service_unbind);
	if (ret) {
		LPERROR("Failed to create endpoint.\r\n");
		return -1;
	}

	LPRINTF("Successfully created rpmsg endpoint.\r\n");
	while(1) {
		platform_poll(priv);
		/* err or INIT */
		if (shutdown_req ) {
			LPRINTF("got shutdown request \r\n");
			break;
		}
	}
	LPRINTF("destroying rpmsg endpoint \r\n");
	rpmsg_destroy_ept(&lept);

	return 0;
}

/*-----------------------------------------------------------------------------*
 *  Application entry point
 *-----------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
	void *platform;
	struct rpmsg_device *rpdev;
	int ret;

	LPRINTF("Starting application...\r\n");

	/* Initialize platform */
	ret = platform_init(argc, argv, &platform);
	if (ret) {
		LPERROR("Failed to initialize platform.\r\n");
		ret = -1;
	} else {
		rpdev = platform_create_rpmsg_vdev(platform, 0,
						   VIRTIO_DEV_SLAVE,
						   NULL, NULL);
		if (!rpdev) {
			LPERROR("Failed to create rpmsg virtio device.\r\n");
			ret = -1;
		} else {
			app(rpdev, platform);
			platform_release_rpmsg_vdev(rpdev);
			ret = 0;
		}
	}

	LPRINTF("Stopping application...\r\n");
	platform_cleanup(platform);

	return ret;
}
