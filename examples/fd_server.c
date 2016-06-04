#include <fd_picotcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>

#include <sys/stat.h>

#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char* argv[]) {
	int sfd1;
	int cfd1;
	int err, n, addl;
	char buffer[100] = "";

	if (argc != 3) {
		printf("USAGE: %s, <MY_IP> <MY_PORT>\n", argv[0]);
		return -1;
	}

	struct sockaddr_in local, client;
	local.sin_family = AF_INET;
	local.sin_port = htons(atoi(argv[2]));
	local.sin_addr.s_addr = inet_addr(argv[1]);

	sfd1 = socket(AF_PICO_INET, SOCK_STREAM, 0);
	if (sfd1 == -1) {
		printf("Error in Open");
		fd_perror();
		return 1;
	}

	err = bind(sfd1, (struct sockaddr*)&local, sizeof(local));	
	if (err == -1) {
		printf("BIND ERROR\n");
		return 1;
	}

	err = listen(sfd1, 3);
	if (err == -1) {
		printf("LISTEN ERROR\n");
		return 1;
	}

	printf("Accepting...\n");

	memset(&client, 0, sizeof(client));
	n = sizeof(client);
	cfd1 = accept(sfd1, (struct sockaddr*)&client, &addl);

	printf("Address Len Returned: %d\n", addl);
	printf("Address Family Returned: %d\n", client.sin_family);
	printf("Address Port Returned: %d\n", client.sin_port);
	printf("Address Returned: %s\n", inet_ntoa(client.sin_addr));

	printf("Connected\n");

	while(1) {
		n = read(cfd1, buffer, 100);
		buffer[n] = 0;
		printf("[fd_socket]%s (%d chars)\n", buffer, n);
	}

	close(cfd1);

	return 0;	
}
