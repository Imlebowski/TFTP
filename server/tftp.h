#ifndef TFTP_H
#define TFTP_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#pragma pack(1)

#define MAX_BUFF 514
#define PORT 20000
#define IP_ADDR "127.0.0.1"
#define RRQ 1
#define WRQ 2
#define DATA 3
#define ACK 4
#define ERR 5

/* structure of udp_packet */
typedef struct 
{
	unsigned short opcode;
	union
	{
		char buff[MAX_BUFF];
		struct
		{
			union
			{
				unsigned short int block_no;
				unsigned short int err_code;
			};
			char data[MAX_BUFF - 2];
		};
	};
}tftp_pkt;

/* buffer */
char buff[MAX_BUFF];

/* socket file descriptors */
int serv_fd, sock_fd, err;

tftp_pkt send_pkt, rec_pkt, ack_pkt, err_pkt;

unsigned short int block, ack_blk;
/*memset(&send_pkt, '\0', sizeof (send_pkt));
memset(&rec_pkt, '\0', sizeof (rec_pkt));
memset(&ack_pkt, '\0', sizeof (ack_pkt));
memset(&err_pkt, '\0', sizeof (err_pkt));*/
struct timeval tv;
fd_set rfd;
int retval;

struct sockaddr_in server_addr, sender_addr, client_addr, host_addr;
socklen_t serv_len, sender_len, client_len, host_len;
void server_process(short int opcode);
#endif

















