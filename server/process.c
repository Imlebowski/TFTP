#include "tftp.h"
extern int errno;

void server_process(short opcode)
{
	block = 1;	
	unsigned int bytes, t_bytes;
	char *temp;
	int fd;
	
	switch (opcode)
	{
		case RRQ:
			{
				temp = strtok(rec_pkt.buff, "0");

				if ((fd = open(temp, O_RDONLY)) == -1)
				{
					server_process(ERR);
					client_len = sizeof (client_addr);
					sendto(sock_fd, (void *)&err_pkt, sizeof (err_pkt), 0, (struct sockaddr*)&client_addr, client_len);
					return;
				}

				send_pkt.opcode = DATA;
				while (1)
				{
					if ((bytes = read(fd, (void *)&(send_pkt.data), sizeof (send_pkt.data))) == -1)
					{
						errno = 1000;
						strcpy(err_pkt.data, "Read failure");
						server_process(ERR);
						return;
					}

					send_pkt.block_no = block++;
					while (1)
					{
						client_len = sizeof(client_addr);
						sendto(sock_fd, (void*)&send_pkt, bytes + 4, 0, (struct sockaddr*)&client_addr, client_len);
						FD_ZERO(&rfd);
						FD_SET(sock_fd, &rfd);

						tv.tv_sec = 2;
						tv.tv_usec = 500;

						retval = select(sock_fd + 1, &rfd, NULL, NULL, &tv);

						if (retval == -1)
						{
							perror("Select");
							break;
						}
						else if (!retval)
						{
							printf("Timed out\n");
							printf("Sending %hd\n", send_pkt.block_no);
							continue;
						}
						else
						{
							
							recvfrom(sock_fd, (void *)&ack_pkt, 4, 0, NULL, NULL);
							printf("Acknowledging %hd\n", ack_pkt.block_no);
							if (ack_pkt.opcode == ACK)
							{
								if (ack_pkt.block_no == (block - 1))
									break;
								else
									continue;
							}
							else if (ack_pkt.opcode == ERR)
								break;
						}
					}

					if (bytes < MAX_BUFF - 2)
						break;

				}

				close(fd);
				break;
			}
		case WRQ:
			{
				temp = strtok(rec_pkt.buff, "0");
				if ((fd = open(temp, O_WRONLY | O_CREAT | O_EXCL, 00642)) == -1)
				{
					server_process(ERR);
					printf("%hd\n", err_pkt.err_code);
					client_len = sizeof(client_addr);
					sendto(sock_fd, (void *)&err_pkt, sizeof (err_pkt), 0, (struct sockaddr*)&client_addr, client_len);
					return;
				}

				ack_pkt.opcode = ACK;
				client_len = sizeof(client_addr);
				sendto(sock_fd, (void *)&ack_pkt, 4, 0, (struct sockaddr*)&client_addr, client_len);

				while (1)
				{
					FD_ZERO(&rfd);
					FD_SET(sock_fd, &rfd);

					tv.tv_sec = 1;
					tv.tv_usec = 0;

					retval = select(sock_fd + 1, &rfd, NULL, NULL, &tv);

					if (retval == -1)
					{
						perror("Select");
						break;
					}
					else if (!retval)
					{
						sendto(sock_fd, (void *)&ack_pkt, 4, 0, (struct sockaddr*)&client_addr, client_len);
						break;
					}
					else
					{
						sender_len = sizeof (sender_addr);
						bytes = recvfrom(sock_fd, (void *)&rec_pkt, sizeof (rec_pkt), 0, (struct sockaddr*)&sender_addr, &sender_len);

						if (rec_pkt.opcode == DATA)
						{

							if ((write(fd, (void *)rec_pkt.data, bytes - 4)) == -1)
							{
								server_process(ERR);
								return;
							}

							ack_pkt.opcode = ACK;
							ack_pkt.block_no = '0';
							sendto(sock_fd, (void *)&ack_pkt, 4, 0, (struct sockaddr*)&sender_addr, sender_len);
						}
					}

					if (bytes < MAX_BUFF - 2)
						break;

				}
				close(fd);
				break;
			}
		case ERR:
			{
				/* sending err_packet */
				err_pkt.opcode = ERR;

				switch (errno)
				{
					case ENOENT:
						err_pkt.err_code = 1;
						break;
					case EACCES:
						err_pkt.err_code = 2;
						break;
					case ENOSPC:
						err_pkt.err_code = 3;
						break;
					case EEXIST:
						err_pkt.err_code = 6;
						break;
					case 1000:
						err_pkt.err_code = 0;
						break;
				}
				if (errno != 1000)
					strcpy (err_pkt.data, strerror(errno));

				return;
			}
		default:
			printf("Default\n");
	}
}
