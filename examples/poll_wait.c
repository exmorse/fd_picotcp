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
	int sfd, cfd;
	int sk, ck;

	int err, n, addl, ris;
	char buffer[100] = "";

	struct sockaddr_in fd_local, fd_client;
	struct sockaddr_in k_local, k_client;
	
	/* fd_picotcp socket */
	fd_local.sin_family = AF_INET;
	fd_local.sin_port = htons(5000);
	fd_local.sin_addr.s_addr = inet_addr("10.0.0.100");

	sfd = socket(AF_PICO_INET, SOCK_STREAM, 0);
	if (sfd == -1) {
		printf("Error in Open");
		fd_perror();
		return 1;
	}

	err = bind(sfd, (struct sockaddr*)&fd_local, sizeof(fd_local));	
	if (err == -1) {
		printf("fd_picotcp bind error\n");
		return 1;
	}

	err = listen(sfd, 3);
	if (err == -1) {
		printf("fd_picotcp listen error\n");
		return 1;
	}

	printf("Accepting on fd_picotcp...\n");

	memset(&fd_client, 0, sizeof(fd_client));
	n = sizeof(fd_client);
	cfd = accept(sfd, (struct sockaddr*)&fd_client, &addl);

	printf("fd_picotcp socket connected\n");


	/* kernel socket */
	memset (&k_local, 0, sizeof(k_local));    
	k_local.sin_family = AF_INET ;   
        k_local.sin_addr.s_addr = htonl(INADDR_ANY); 
        k_local.sin_port = htons(5000); 

	sk = socket(AF_INET, SOCK_STREAM, 0); 
        if (sk == -1) { 
                perror("SOCKET");
                return 2 ; 
        }

	printf("Binding\n");	
	ris = bind(sk, (struct sockaddr*)&k_local, sizeof(k_local)); 
        if (ris == -1) { 
                perror("BIND");
                return 3 ; 
        }

	printf("Listening\n");
	ris = listen(sk, 10);
        if (ris == -1) {
                perror("LISTEN");
                return 4 ;
        }

	memset(&k_client, 0, sizeof(k_client)) ;
        addl = sizeof(k_client);
        printf ("Accepting...\n");
        ck = accept(sk, (struct sockaddr*) &k_client , &addl);

        printf("Connection from %s : %d\n", inet_ntoa(k_client.sin_addr) , ntohs(k_client.sin_port));

	/* Poll structure */
	struct pollfd fds[2];
        fds[0].fd = cfd;
        fds[0].events = POLLIN;
        fds[1].fd = ck;
        fds[1].events = POLLIN;

	while(1) {
		printf("Polling...\n");
		poll(fds, 2, -1);
                printf("Poll Passata\n");

		if (fds[0].revents) {    
                        n = read(cfd, buffer, 1024);
                        buffer[n] = 0;
                        printf("[Client_Pico_Sock]: %s\n", buffer);
                }

                else if (fds[1].revents) {
                        n = read(ck, buffer, 1024);
                        buffer[n] = 0;
                        printf("[Client_Kernel]: %s\n", buffer);
                }
	}

	close(cfd);
	close(ck);
	return 0;	
}
