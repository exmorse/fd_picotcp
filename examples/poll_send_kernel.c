#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define SOCKET_ERROR ((int) -1 )

int main(int argc, char* argv[]) {

        int sock_fd, i, n, ris;
        short int local_port, server_port;
        struct sockaddr_in local, server;
        char string_server_ip [40] = "127.0.0.1";

        server_port = 5000;

        sock_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (sock_fd == SOCKET_ERROR) {
                printf ( "Error creating socket\n" ) ;
                return 2;
        }

        memset(&local, 0, sizeof(local));

        local.sin_family = AF_INET ;  
        local.sin_addr.s_addr = htonl(INADDR_ANY);
        local.sin_port = htons(0);

        ris = bind(sock_fd ,(struct sockaddr *)&local, sizeof(local));
        if (ris == SOCKET_ERROR) {
                printf("Error in bind.\n");
                return 3;
        }

        memset(&server, 0, sizeof(server));
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = inet_addr(string_server_ip);
        server.sin_port = htons(server_port);

        ris = connect(sock_fd ,(struct sockaddr *)&server, sizeof(server));
        if (ris == SOCKET_ERROR) {
                printf ( "Error in connect\n" ) ;
                return 4 ;
        }

        char buffer[1024];
        int buffer_len = 0;

        while (1) {
                printf("-->");
                fflush(stdout);
                buffer_len = read(STDIN_FILENO, buffer, 1024);
                if (buffer_len > 0) {
			buffer[buffer_len-1] = 0;
                        write(sock_fd, buffer, buffer_len);
                }
        }

        close(sock_fd);
        printf("Closing\n");
        return 0 ;
}


