/**
	socket.c

	This file contains functions used to manipulate both fd_picotcp sockets
	and kernel sockets.
	These functions override the standard library functions, so names, arguments 
	and return values are the same as the standard version.

	If the socket file descriptor passed ad argument is a fd_picotcp socket then 
	function from fd_function.c are	called, else the request is handled via 
	system call (invoked using "syscall()", to avoid an infinite loop).

	To create a fd_socket call "socket()" with AF_PICO_INET or AF_PICO_IFNET6 as
	first parameter.
**/

#define _GNU_SOURCE

#include "fd_picotcp.h"
#include "fd_extern.h"
#include "fd_functions.h"

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
#include <linux/net.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#ifndef AF_PICO_INET 
        #define AF_PICO_INET 1018
#endif

#ifndef AF_PICO_INET6 
        #define AF_PICO_INET6 1019
#endif


ssize_t read(int fd, void* buf, size_t len) {
	if (get_socket_from_fd(fd) != NULL) {
		return fd_pico_socket_read(fd, buf, len);
	}
	
	return syscall(__NR_read, fd, buf, len);
}

ssize_t write(int fd, const void* buf, size_t count) {
	if (get_socket_from_fd(fd) != NULL) {
		return fd_pico_socket_write(fd, buf, count);
	}

	return syscall(__NR_write, fd, buf, count);
}

int listen(int fd, int backlog) {
	if (get_socket_from_fd(fd) != NULL) {
		return fd_pico_socket_listen(fd, backlog);
	}
	
	#ifdef __NR_listen
	return syscall(__NR_listen, fd, backlog);
	#else
	unsigned long args[2] = {fd, backlog};
	return syscall(__NR_socketcall, SYS_LISTEN, fd, backlog);
	#endif
}

int close(int fd) {
	if (get_socket_from_fd(fd) != NULL) {
		return fd_pico_socket_close(fd);
	}
	
	return syscall(__NR_close, fd);
}

/* The socket file descriptor is returned */
int socket(int domain, int type, int protocol) {

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
	#ifdef __NR_socket
	return syscall(__NR_socket, domain, type, protocol);
	#else
	unsigned long args[3] = {domain, type, protocol};
	return syscall(__NR_socketcall, SYS_SOCKET, args);
	#endif
}

int bind(int fd, const struct sockaddr* addr, socklen_t addrlen) {
	if (get_socket_from_fd(fd) == NULL) {
		#ifdef __NR_bind
		return syscall(__NR_bind, fd, addr, addrlen);
		#else
		unsigned long args[3] = {fd, (unsigned long)addr, addrlen};
		return syscall(__NR_socketcall, SYS_BIND, args);
		#endif
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

int connect (int fd, const struct sockaddr* addr, socklen_t addrlen) {
	if (get_socket_from_fd(fd) == NULL) {
		#ifdef __NR_connect
		return syscall(__NR_connect, fd, addr, addrlen);
		#else
		unsigned long args[3] = {fd, (unsigned long)addr, addrlen};
		return syscall(__NR_socketcall, SYS_CONNECT, args);
		#endif
	}
	
	/* fd_picotcp IPv4 socket */
	if (addrlen == sizeof(struct sockaddr_in)) {
		struct sockaddr_in* server = (struct sockaddr_in*)addr;
		char * addr_string = inet_ntoa(server->sin_addr);
		if (addr_string == NULL) {
			fd_err = FD_INVALID_ADDRESS;
			return -1;
		}	
		return fd_pico_socket_connect(fd, addr_string, server->sin_port);
	}
	
	/* fd_picotcp IPv6 socket */
	else if (addrlen == sizeof(struct sockaddr_in6)) {
		char addr_string[100];
		struct sockaddr_in6* server = (struct sockaddr_in6*)addr;
		inet_ntop(AF_INET6, &server->sin6_addr, addr_string, addrlen);
	
		if (addr_string == NULL) {
			fd_err = FD_INVALID_ADDRESS;
			return -1;
		}		
		
		return fd_pico_socket_connect(fd, addr_string, server->sin6_port);
	}
	
	else return -1;
}

int accept (int fd, struct sockaddr* addr, socklen_t* addrlen) {
	if (get_socket_from_fd(fd) == NULL) {
		#ifdef __NR_accept
		return syscall(__NR_accept, fd, addr, addrlen);
		#else
		unsigned long args[3] = {fd, (unsigned long)addr, addrlen};
		return syscall(__NR_socketcall, SYS_ACCEPT, args);
		#endif
	}

	int* p_port = (int*)malloc(sizeof(int));
	char str_addr[100];

	/* Blocking */
	int ret_fd = fd_pico_socket_accept(fd, str_addr, p_port);
	
	fd_elem* l = get_fd_elem_from_fd(ret_fd);
	
	if (l == NULL) {
		addr = NULL;
		*addrlen = 0;
		return -1;
	} 

	/* fd_picotcp IPv4 socket */
	if (!l->isIpv6) {
		struct sockaddr_in* s = (struct sockaddr_in*)addr;
		s->sin_family = AF_INET;
		s->sin_port = *p_port;
		s->sin_addr.s_addr = inet_addr(str_addr);
		*addrlen = sizeof(*s);	
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
