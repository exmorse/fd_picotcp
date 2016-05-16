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
		printf("USAGE: %s, <MY_IPv6> <MY_PORT>\n", argv[0]);
		return -1;
	}

	struct sockaddr_in6 local, client;
	local.sin6_family = AF_INET6;
	local.sin6_port = htons(atoi(argv[2]));
	err = inet_pton(AF_INET6, argv[1], &local.sin6_addr);
	if (err == 0 || err == -1) {
		printf("Address not valid\n");
		return 1;
	}

	sfd1 = socket(AF_PICO_INET6, SOCK_STREAM, 0);
	if (sfd1 == -1) {
		printf("Error in Open");
		return 1;
	}

	err = bind(sfd1, (struct sockaddr*)&local, sizeof(local));	
	if (err == -1) {
		printf("BIND %s\n", strerror(pico_err));
		return 1;
	}

	err = listen(sfd1, 3);
	if (err == -1) {
		printf("%s\n", strerror(pico_err));
		return 1;
	}

	printf("Accepting...\n");

	memset(&client, 0, sizeof(client));
	n = sizeof(client);
	cfd1 = accept(sfd1, (struct sockaddr*)&client, &addl);

	char addr_string[100];
	printf("Address Len Returned: %d\n", addl);
	printf("Address Family Returned: %d\n", client.sin6_family);
	printf("Address Port Returned: %d\n", client.sin6_port);
	inet_ntop(AF_INET6, &client.sin6_addr, addr_string, addl);
	printf("Address Returned: %s\n", addr_string);

	printf("Connected\n");

	while(1) {
		n = read(cfd1, buffer, 100);
		buffer[n] = 0;
		printf("[fd_socket1]%s (%d chars)\n", buffer, n);
	}

	close(cfd1);

	return 0;	
}
