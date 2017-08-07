#include "tftp.h"

void client_process(short int opcode)
{
	block = 1;
	unsigned short int bytes, idx = 0;
	unsigned int t_bytes = 0;
	char *temp;
	int fd;

	switch (opcode)
	{
		case RRQ:
			{
				int idx = 0;

				if ((fd = open(&buff[4], O_WRONLY | O_CREAT | O_EXCL, 00642)) == -1)
				{
					perror("Open");	
					return;
				}
				/* filling the send packet */
				send_pkt.opcode = RRQ;
				strtok(buff, " ");
				temp = strtok(NULL, " ");
				strcpy(send_pkt.buff, temp);
				//send_pkt.buff[strlen(temp) + 1] = 48;
				strcpy(&(send_pkt.buff[strlen(temp) + 2]), "netascii");

				printf("file name : %s\n", send_pkt.buff);
				printf("mode : %s\n", &(send_pkt.buff[strlen(temp) + 2]));
				//send_pkt.buff[strlen(temp) + strlen("netascii") + 2] = 48;

				/* sending the request cum connect packet to the server */
				serv_len = sizeof (server_addr);
				sendto(sock_fd, (void *)&send_pkt, (size_t)sizeof (send_pkt), 0, (struct sockaddr*)&server_addr, serv_len);


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
						printf("Timed out\nWaiting again\n");
						continue;
					}
					else
					{
						sender_len = sizeof (sender_addr);
						bytes = recvfrom(sock_fd, (void *)&rec_pkt, sizeof (rec_pkt), 0, (struct sockaddr*)&sender_addr, &sender_len);

						if (rec_pkt.opcode == DATA)
						{
							printf("Received data block %d\t", rec_pkt.block_no);

							if (rec_pkt.block_no != ack_pkt.block_no)
							{
								if ((write(fd, (void *)rec_pkt.data, bytes - 4)) == -1)
								{
									perror("Write");
									return;
								}
							}

							printf("Sending acknowledgement packet\n");
							ack_pkt.opcode = ACK;
							ack_pkt.block_no = rec_pkt.block_no;

								//sleep(3);

							if (idx != 100)
							sendto(sock_fd, (void *)&ack_pkt, 4, 0, (struct sockaddr*)&sender_addr, sender_len);
							idx++;
						}
						else if (rec_pkt.opcode == ERR)
						{
							err_pkt = rec_pkt;
							client_process(ERR);
							return;
						}

						//printf("size %hd\n", bytes);
						t_bytes += (bytes);

						if (bytes < MAX_BUFF - 2)
							break;
					}
				}
					printf("Total bytes %d\n", t_bytes);
					close(fd);
			}
			break;
		case WRQ:
			{
				/* filling the send packet */
				send_pkt.opcode = WRQ;
				strtok(buff, " ");
				temp = strtok(NULL, " ");
				strcpy(send_pkt.buff, temp);
				send_pkt.buff[strlen(temp) + 1] = 48;
				strcpy(&(send_pkt.buff[strlen(temp) + 2]), "netascii");

				printf("file name : %s\n", send_pkt.buff);
				printf("mode : %s\n", &(send_pkt.buff[strlen(temp) + 2]));
				send_pkt.buff[strlen(temp) + strlen("netascii") + 2] = 48;

				/* sending the request cum connect packet to the server */
				printf("Requesting for connection\t");
				serv_len = sizeof (server_addr);
				sendto(sock_fd, (void *)&send_pkt, (size_t)sizeof (send_pkt), 0, (struct sockaddr*)&server_addr, serv_len);

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
					printf("Connection timed out\n");
					break;
				}
				else
				{
					sender_len = sizeof (sender_addr);
					recvfrom(sock_fd, (void *)&ack_pkt, (size_t)sizeof (ack_pkt), 0, (struct sockaddr*)&sender_addr, &sender_len);

					if (ack_pkt.opcode == ACK)
					{
						printf("Acknowledged\n");

						if ((fd = open(temp, O_RDONLY)) == -1)
						{
							perror("Open");
							return;
						}

						send_pkt.opcode = DATA;

						while (1)
						{
							if ((bytes = read(fd, (void *)&(send_pkt.data), sizeof (send_pkt.data))) == -1)
							{
								perror("Read");
								return;
							}

							send_pkt.block_no = block++;

							while (1)
							{
								printf("Sending data of block %d\t", block - 1);

								sender_len = sizeof(sender_addr);
								sendto(sock_fd, (void*)&send_pkt, bytes + 4, 0, (struct sockaddr*)&sender_addr, sender_len);

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
									printf("Acknowledgment not recieved\n");
									break;
								}
								else
								{

									recvfrom(sock_fd, (void *)&ack_pkt, sizeof(ack_pkt), 0, NULL, NULL);
									if (ack_pkt.opcode == ACK)
									{
										printf("Acknowledged\n");
										break;
									}
									else if (ack_pkt.opcode == ERR)
										client_process(ERR);
									else 
										continue;
								}
							}

							t_bytes += (bytes - 4);

							if (bytes < MAX_BUFF - 2)
								break;
						}
						close(fd);
						printf("Total bytes sent = %d\n", t_bytes);
					}
					else if (ack_pkt.opcode == ERR)
					{
						err_pkt = ack_pkt;
						client_process(ERR);
					}
				}
				break;
			}
		case ERR:
			{
				switch (err_pkt.err_code)
				{
					case 1:
						printf("File Not found\n");
						break;
					case 2:
						printf("Access violation\n");
						break;
					case 3:
						printf("No space\n");
						break;
					case 6:
						printf("File already exist\n");
						break;
					case 0:
						printf("%s\n", err_pkt.data);
						break;
				}
			}
			break;
		default:
			printf("Default\n");
	}
}
