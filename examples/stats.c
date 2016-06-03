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

	int i = 0;
	for (i = 0; i < 100; i++) {
		sfd1 = socket(AF_PICO_INET, SOCK_STREAM, 0);
		sfd1 = socket(AF_INET, SOCK_STREAM, 0);
		printf("%d\n", i);
		if (sfd1 == -1) {
			printf("Error in Open");
			return 1;
		}
	}
	_print_stats();
	fd_perror();
}
