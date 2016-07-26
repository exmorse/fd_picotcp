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

	struct sockaddr_in local, server;

	cfd = socket(AF_PICO_INET, SOCK_STREAM, 0);

	local.sin_family = AF_INET;
	local.sin_port = htons(5000);
	local.sin_addr.s_addr = inet_addr("10.0.0.200");
	err = bind(cfd, (struct sockaddr*)&local, sizeof(local));
	if (err == -1) {
		printf("BIND ERROR\n");
	}

	server.sin_family = AF_INET;
	server.sin_port = htons(5000);
	server.sin_addr.s_addr = inet_addr("10.0.0.100");
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
