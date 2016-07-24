/**
	fede_socket.c

	Test file to be used in pycotcp.
**/

#define _GNU_SOURCE

#include "fd_picotcp.h"
#include "fd_extern.h"
#include "fd_functions.h"
#include "fede_socket.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h> 
#include <stdarg.h>
#include <string.h>

#include <sys/syscall.h>

#include <pico_stack.h>
#include <pico_dev_vde.h>
#include <pico_socket.h>
#include <pico_ipv6.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#ifndef AF_PICO_INET 
        #define AF_PICO_INET 1018
#endif

#ifndef AF_PICO_INET6 
        #define AF_PICO_INET6 1019
#endif


ssize_t pico_read(int fd, void* buf, size_t len) {
	if (get_socket_from_fd(fd) != NULL) {
		return fd_pico_socket_read(fd, buf, len);
	}
	
	return syscall(__NR_read, fd, buf, len);
}

ssize_t pico_write(int fd, const void* buf, size_t count) {
	if (get_socket_from_fd(fd) != NULL) {
		return fd_pico_socket_write(fd, buf, count);
	}

	return syscall(__NR_write, fd, buf, count);
}

int pico_listen(int fd, int backlog) {
	if (get_socket_from_fd(fd) != NULL) {
		return fd_pico_socket_listen(fd, backlog);
	}
	
	return syscall(__NR_listen, fd, backlog);
}

int pico_close(int fd) {
	if (get_socket_from_fd(fd) != NULL) {
		return fd_pico_socket_close(fd);
	}
	
	return syscall(__NR_close, fd);
}

/* The socket file descriptor is returned */
int pico_socket(int domain, int type, int protocol) {
	
	/* fd_picotcp IPv4 socket */
	if (domain == AF_PICO_INET) {
		if (type == SOCK_STREAM)
			return fd_pico_socket_open(PICO_PROTO_IPV4, PICO_PROTO_TCP);
		else if (type == SOCK_DGRAM)
			return fd_pico_socket_open(PICO_PROTO_IPV4, PICO_PROTO_UDP);
		else 
			return -1;
	}
	
	/* fd_picotcp IPv6 socket */
	if (domain == AF_PICO_INET6) {
		if (type == SOCK_STREAM)
			return fd_pico_socket_open(PICO_PROTO_IPV6, PICO_PROTO_TCP);
		else if (type == SOCK_DGRAM)
			return fd_pico_socket_open(PICO_PROTO_IPV6, PICO_PROTO_UDP);
		else 
			return -1;
	}
	
	/* Kernel stack socket */
	return syscall(__NR_socket, domain, type, protocol);
}

int pico_bind(int fd, const struct sockaddr* addr, socklen_t addrlen) {
	if (get_socket_from_fd(fd) == NULL) {
		return syscall(__NR_bind, fd, addr, addrlen);
	}

	/* fd_picotcp IPv4 socket */
	if (addrlen == sizeof(struct sockaddr_in)) {
		struct sockaddr_in* local = (struct sockaddr_in*)addr;
		char * addr_string = inet_ntoa(local->sin_addr);
		if (addr_string == NULL) {
			fd_err = FD_INVALID_ADDRESS;
			return -1;
		}
		return fd_pico_socket_bind(fd, addr_string, local->sin_port);
	}
	
	/* fd_picotcp IPv6 socket */
	else if (addrlen == sizeof(struct sockaddr_in6)) {
		char addr_string[100];
		struct sockaddr_in6* local = (struct sockaddr_in6*)addr;
		inet_ntop(AF_INET6, &local->sin6_addr, addr_string, addrlen);
	
		if (addr_string == NULL) {
			fd_err = FD_INVALID_ADDRESS;
			return -1;
		}		

		return fd_pico_socket_bind(fd, addr_string, local->sin6_port);
	}

	else return -1;
}

int pico_connect (int fd, const struct sockaddr* addr, socklen_t addrlen) {
	if (get_socket_from_fd(fd) == NULL) {
		return syscall(__NR_connect, fd, addr, addrlen);
	}
	
	/* fd_picotcp IPv4 socket */
	if (addrlen == sizeof(struct sockaddr_in)) {
		struct sockaddr_in* local = (struct sockaddr_in*)addr;
		char * addr_string = inet_ntoa(local->sin_addr);
		if (addr_string == NULL) {
			fd_err = FD_INVALID_ADDRESS;
			return -1;
		}	
		return fd_pico_socket_connect(fd, addr_string, local->sin_port);
	}
	
	/* fd_picotcp IPv6 socket */
	else if (addrlen == sizeof(struct sockaddr_in6)) {
		char addr_string[100];
		struct sockaddr_in6* local = (struct sockaddr_in6*)addr;
		inet_ntop(AF_INET6, &local->sin6_addr, addr_string, addrlen);
	
		if (addr_string == NULL) {
			fd_err = FD_INVALID_ADDRESS;
			return -1;
		}		
		
		return fd_pico_socket_connect(fd, addr_string, local->sin6_port);
	}
	
	else return -1;
}

int pico_accept (int fd, struct sockaddr* addr, 
		socklen_t* addrlen) {
	if (get_socket_from_fd(fd) == NULL) {
		return syscall(__NR_accept, fd, addr, addrlen);
	}

	int* p_port = (int*)malloc(sizeof(int));
	char str_addr[100];
	int ret_fd = fd_pico_socket_accept(fd, str_addr, p_port);
	
	printf("--- Accept returned : %d ---\n", ret_fd);
	
	fd_elem* l = get_fd_elem_from_fd(ret_fd);
	
	printf("--- GOT ---\n");
	
	if (l == NULL) {
		addr = NULL;
		*addrlen = 0;
		printf("--- Accept is going to be NULL ---\n");
		return -1;
	} 

	/* fd_picotcp IPv4 socket */
	if (!l->isIpv6) {
		printf("--- IF BRANCH ---\n");
		struct sockaddr_in* s = (struct sockaddr_in*)addr;
		if (s == NULL) printf("--- s is NULL ---\n");
		printf("--- sockaddr ---\n");
		s->sin_family = AF_INET;
		printf("--- af_inet ---\n");
		s->sin_port = *p_port;
		printf("--- sin_port ---\n");
		s->sin_addr.s_addr = inet_addr(str_addr);
		printf("--- s_addr ---\n");
		//*addrlen = (socklen_t)sizeof(*s);
		printf("--- sizeof %d ---\n", (int)sizeof(*s));
	
		printf("--- Accept will return : %d ---\n", ret_fd);
			
		return ret_fd;
	} 
		
	/* fd_picotcp IPv6 socket */
	if (l->isIpv6) {
		struct sockaddr_in6* s = (struct sockaddr_in6*)addr;
		s->sin6_family = AF_INET6;
		s->sin6_port = *p_port;
		inet_pton(AF_INET6, str_addr, &s->sin6_addr);
		*addrlen = sizeof(*s);	
		return ret_fd;
	} 
		
	return ret_fd;
}	
