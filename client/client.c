#include "tftp.h"

int main()
{

	/* temp mem for ip address */ 
	char *ip_addr = (char *)calloc(15, sizeof (char));

	/* creating socket*/
	if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		perror("Socket creation");
		exit(0);
	}


	/* clearing the structure */
	memset(&server_addr, 0, sizeof (struct sockaddr_in));

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(6900);
	
	printf("connect ");
	scanf("%s", ip_addr);
	
	server_addr.sin_addr.s_addr = inet_addr(ip_addr);
	//printf("%s\n", ip_addr);

	while (1)
	{
		memset(buff, '\0', sizeof (buff));
		printf("<tftp> ");
		scanf("\n%[^\n]", buff);

		if (!strncmp(buff, "bye", 3))
			break;
		else if (!strncmp(buff, "get", 3))
			client_process(RRQ);
		else if (!strncmp(buff, "put", 3))
			client_process(WRQ);
		else
			printf("Invalid command\n");
	}

	return 96;
}
