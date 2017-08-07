#include "tftp.h"

int main()
{
	int idx = 0;

	/* creating socket*/
	if ((serv_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		perror("Socket creation");
		exit(0);
	}

	/* clearing the structure */
	memset(&server_addr, 0, sizeof (struct sockaddr_in));
	memset(&client_addr, 0, sizeof (struct sockaddr_in));
	memset(&host_addr, 0, sizeof (struct sockaddr_in));

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(6900);
	server_addr.sin_addr.s_addr = inet_addr(IP_ADDR);
	serv_len = sizeof (server_addr);

	if ((bind(serv_fd, (struct sockaddr *)&server_addr, serv_len))  == -1)
	{
		perror("bind1");
		exit(1);
	}

  memset(&send_pkt, '\0', sizeof (send_pkt));
  memset(&rec_pkt, '\0', sizeof (rec_pkt));
  memset(&ack_pkt, '\0', sizeof (ack_pkt));
  memset(&err_pkt, '\0', sizeof (err_pkt));

	while (++idx)
	{
	client_len = sizeof (client_addr);

	if ((recvfrom(serv_fd, (void *)&rec_pkt, (size_t)sizeof (rec_pkt), 0, (struct sockaddr *)&client_addr, &client_len)) == -1)
	{
		perror("recieve");
		exit(2);
	}

		switch (fork())
		{
			case -1:
				perror("Fork");
				exit(1);
			case 0:
				if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
				{
					perror("Socket creation : child");
					exit(3);
				}

				host_addr.sin_family = AF_INET;
				host_addr.sin_port = htons(PORT + idx);
				host_addr.sin_addr.s_addr = inet_addr(IP_ADDR);

				host_len = sizeof (host_addr);

				if ((bind(sock_fd, (struct sockaddr *)&host_addr, host_len))  == -1)
				{
					perror("bind child");
					exit(4);
				}
				server_process(rec_pkt.opcode);
			default:
				continue;
		}

		return 69;
	}
}
