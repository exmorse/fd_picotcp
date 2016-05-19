#include <fd_picotcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>

#include <sys/stat.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

int main(int argc, char* argv[]) {
	int sfd;
	int cfd;
	int err, n;	
	char buffer[100];

	struct sockaddr_in6 local, server;

	if (argc != 4) {
		printf("USAGE: %s <MY_IPv6> <SERVER_IPv6> <SERVER_PORT>\n", argv[0]);
		return -1;
	}

	cfd = socket(AF_PICO_INET6, SOCK_STREAM, 0);

	local.sin6_family = AF_INET6;
	local.sin6_port = htons(5000);
	err = inet_pton(AF_INET6, argv[1], &local.sin6_addr);
        if (err == 0 || err == -1) printf("***inet_pton error\n");

	err = bind(cfd, (struct sockaddr*)&local, sizeof(local));
	if (err == -1) {
		printf("BIND ERROR\n");
	}

	server.sin6_family = AF_INET6;
	server.sin6_port = htons(atoi(argv[3]));
	err = inet_pton(AF_INET6, argv[2], &server.sin6_addr);
        if (err == 0 || err == -1) printf("***inet_pton error\n");
	
	err = connect(cfd, (struct sockaddr*)&server, sizeof(server));
	if (err == -1) {
		printf("CONNECT ERROR\n");
	}

	while(1) {
		printf("-->");
		fflush(stdout);
		n = read(STDIN_FILENO, buffer, 100);
		buffer[n-1] = 0;
		n = write(cfd, buffer, n);
	}
	
	return 0;	
}
