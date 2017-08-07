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
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#pragma pack(1)

#define MAX_BUFF 514
#define PORT 20000
#define IP_ADDR "192.168.43.150"
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
int sock_fd, data_fd;
tftp_pkt send_pkt, rec_pkt, ack_pkt, err_pkt;
unsigned short int block;

struct timeval tv;
fd_set rfd;
int retval;

struct sockaddr_in server_addr, sender_addr;
socklen_t serv_len, sender_len;
void client_process(short int opcode);
int process(short int opcode);
#endif
