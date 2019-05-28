#ifndef RPMSG_ECHO_H
#define RPMSG_ECHO_H

#define RPMSG_SERVICE_NAME         "rpmsg-openamp-demo-channel"

#define OUT_OF_BAND (0x1UL<<31)
#define INIT_MSG 0x2UL
#define ACK_MSG 0X3UL
#define DATA_MSG 0x4UL
#define SHUTDOWN_MSG 0x5UL
#define TABLE_BASE_ADDRESS 0x3ee20000UL
#define BUFFER_SIZE 0x10000UL //64K.. can change to 65K as needed

static struct packet {
    unsigned packet_type;
    unsigned buffer_index;
    unsigned packet_length;
};

#define LPRINTF(format, ...) printf(format, ##__VA_ARGS__)
#define LPERROR(format, ...) LPRINTF("ERROR: " format, ##__VA_ARGS__)
#endif /* RPMSG_ECHO_H */
