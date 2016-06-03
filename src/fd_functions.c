/**
	fd_functions.c

	This file contains the functions used to manipulate the picotcp sockets.
	
	They work as an adapter (used by socket.c), making the picotcp sockets behave 
	like the standard sockets.

	In particular "fd_pico_socket_accept()" and "pico_socket_read()" are blocking.
	They may block the process on a semaphore, and "handle_wakeup()" in fd_picotcp.c
	unlock it when the related event is detected.
**/

#define _GNU_SOURCE

#include "fd_picotcp.h"
#include "fd_extern.h"
#include "fd_methods.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h> 
#include <stdarg.h>
#include <string.h>

#include <pico_stack.h>
#include <pico_dev_vde.h>
#include <pico_socket.h>
#include <pico_ipv6.h>

#include <sys/stat.h>

#include <semaphore.h>

#include <sys/syscall.h>

#include <sys/types.h>
#include <sys/socket.h>

static int thread_launched = 0;

int fd_pico_socket_open(uint16_t net, uint16_t proto) {	
	struct pico_socket* s;
	int pipefd[2];
	int err, new_fd;

	/* Check if this is the first call to fd_pico_socket_open */
	/* If so, call init() to setup the VDE environment */
	if (thread_launched == 0) {
		thread_launched++;
		init();
	}

	s = pico_socket_open(net, proto, handle_wakeup);
	if (s == NULL) {
		fd_err = FD_SOCKET_IS_NULL;
		return -1;
	}

	err = pipe(pipefd);
	if (err == -1) {
		fd_err = FD_PIPE_NOT_INITIALIZED;
		return -1;
	}

	/* Create new fd_picotcp socket struct, using pipe and picotcp socket */
	new_fd = fd_elem_create(pipefd, s);
	if (new_fd < 0) {
		//fd_err = FD_TABLE_INSERT_ELEMENT;
		return -1;
	}

	/* Set IP version in socket structure */
	if (net == PICO_PROTO_IPV4) get_fd_elem_from_fd(new_fd)->isIpv6 = 0;
	if (net == PICO_PROTO_IPV6) get_fd_elem_from_fd(new_fd)->isIpv6 = 1 ;
	
	return new_fd;
}

int fd_pico_socket_bind(int fd, char* address, uint16_t port) {
	struct pico_ip4 ip_address = {0};
	struct pico_ip4 netmask;
	struct pico_ip6 ip6_address, ip6_netmask = {{0}};
	struct pico_socket* s = NULL;
	struct pico_ipv6_link* link6 = NULL;
	int err;

	/* Get picotcp socket */
	s = get_socket_from_fd(fd);
	if (s == NULL) {
		fd_err = FD_SOCKET_NOT_FOUND;
		return -1;
	}

	/* IPv4 - picotcp bind*/
	if (isIpv4(address)) {
		err = pico_string_to_ipv4(address, &ip_address.addr);
		if (err == -1) {
			fd_err = FD_INVALID_ADDRESS;
			return -1;
		}
		
		err = pico_string_to_ipv4(ipv4_mask_string, &netmask.addr);
		if (err == -1) {
			fd_err = FD_INVALID_NETMASK;
			return -1;
		}
	
		if (pico_ipv4_link_find(&ip_address) == NULL) {
			/* Add interface to device */	
			err = pico_ipv4_link_add(pico_dev, ip_address, netmask);
			if (err == -1) { 
				fd_err = FD_INTERFACE_NOT_LINKED;
				return -1;
			}
		}

		return pico_socket_bind(s, &ip_address.addr, &port);
	}

	/* IPv6 - picotcp bind */
	else if (isIpv6(address)) {
		err = pico_string_to_ipv6(address, ip6_address.addr);
		if (err == -1) {
			fd_err = FD_INVALID_ADDRESS;
			return -1;
		}
	
		err = pico_string_to_ipv6(ipv6_mask_string, ip6_netmask.addr);
		if (err == -1) {
			fd_err = FD_INVALID_NETMASK;
			return -1;
		}

		if (pico_ipv6_link_find(&ip6_address) == NULL) {
			/* Add interface to device */	
			link6 = pico_ipv6_link_add(pico_dev, ip6_address, ip6_netmask);
			if (link6 == NULL) { 
				fd_err = FD_INTERFACE_NOT_LINKED;
				return -1;
			}
		}

		/* Wait for DAD validation to finish before binding */
		/* A timeout of 3 seconds is set, if it takes longer returns -1 */
		time_t start_time = time(NULL);
		while (pico_ipv6_link_find(&ip6_address) == NULL) {
			if (time(NULL) - start_time >= 3) {
				fd_err = FD_INTERFACE_NOT_LINKED;
				return -1;
			}
			usleep(1000);
		}

		return pico_socket_bind(s, &ip6_address.addr, &port);
	}

	else {
		fd_err = FD_INVALID_ADDRESS;
		return -1;
	}

}

int fd_pico_socket_listen(int fd, int backlog) {
	struct pico_socket* s;

	s = get_socket_from_fd(fd);
	if (s == NULL) {
		fd_err = FD_SOCKET_NOT_FOUND;
		return -1;
	}

	return pico_socket_listen(s, backlog);
}

int fd_pico_socket_connect(int fd, char* address, uint16_t port) {
        struct pico_ip4 ip_address = {0};
        struct pico_ip6 ip6_address = {0};
        struct pico_socket* s;
        int err;
	fd_elem* l;

	l = get_fd_elem_from_fd(fd);
	s = get_socket_from_fd(fd);
        if (s == NULL) {
		fd_err = FD_SOCKET_NOT_FOUND;
                return -1;
        }

        /* IPv4 - picotcp connect */
	if (!l->isIpv6) {
		err = pico_string_to_ipv4(address, &ip_address.addr);
        	if (err == -1) {
			fd_err = FD_INVALID_ADDRESS;
                	return -1;
        	}
	
		return pico_socket_connect(s, &ip_address.addr, port);
	}

	/* IPv6 - picotcp connect*/
	else {
		err = pico_string_to_ipv6(address, ip6_address.addr);
        	if (err == -1) {
			fd_err = FD_INVALID_ADDRESS;
                	return -1;
        	}
	
		return pico_socket_connect(s, &ip6_address.addr, port);
	}

	return -1;
}

int fd_pico_socket_accept(int fd, char* str_addr, int* p_port) {
	uint16_t s_port;  
        struct pico_ip4 ip_address = {0};
        struct pico_ip6 ip6_address = {0};
	fd_elem* l = get_fd_elem_from_fd(fd);
	struct pico_socket* c;
	int pipefd[2];
	int err, new_fd;

	if (l == NULL) {	
		fd_err = FD_SOCKET_NOT_FOUND;
		return -1;
	}
	
	/* Make the accept blocking */
	waiton_sem(l->accept_sem);
	
	/* IPv4 - picotcp accept */
	if (!l->isIpv6) {
		c = pico_socket_accept(l->socket, &ip_address, &s_port);
		if (c == NULL) {
			fd_err = FD_SOCKET_IS_NULL;
			return -1;
		}
		pico_ipv4_to_string(str_addr, ip_address.addr);
	}

	/* IPv6 - picotcp accept */
	else if (l->isIpv6) {
		c = pico_socket_accept(l->socket, &ip6_address, &s_port);
		if (c == NULL) {
			fd_err = FD_SOCKET_IS_NULL;
			return -1;
		}
		pico_ipv6_to_string(str_addr, ip6_address.addr);
	}

	*p_port = s_port;
	
	/* Create a fd_elem structure for the accepted picotcp socket */
	err = pipe(pipefd);
	new_fd = fd_elem_create(pipefd, c);

	if (new_fd < 0) {
		return -1;
	}	

	if (l->isIpv6) {
		fd_elem* nl = get_fd_elem_from_fd(new_fd);
		if (nl != NULL) nl->isIpv6 = 1;
	}

	return new_fd;
}

int fd_pico_socket_write(int fd, const void* buffer, int len) {
	fd_elem *l;
	int err;
	
	l = get_fd_elem_from_fd(fd);
	if (l == NULL) {
		fd_err = FD_SOCKET_NOT_FOUND;
		return -1;
	}

	return pico_socket_write(l->socket, buffer, len);	
}

int fd_pico_socket_read(int fd, char* buffer, int len) {
	fd_elem *l;
	int err, n;

	l = get_fd_elem_from_fd(fd);
	if (l == NULL) {
		fd_err = FD_SOCKET_NOT_FOUND;
		return -1;
	}	

	/* Make the read blocking */
	waiton_sem(l->read_sem);
	
	char b[10];
	
	/* Read chars from the fd_elem pipe until it is empty */
	while (l->fd_char_count > 0) {
		n = syscall(__NR_read, l->fd[0], b, 10); 
		l->fd_char_count -= n;
	}

	return pico_socket_read(l->socket, buffer, len);

}

int fd_pico_socket_close(int fd) {
	struct pico_socket* s;
	int ret;
	
	s = get_socket_from_fd(fd);
	if (s == NULL) {		
		fd_err = FD_SOCKET_NOT_FOUND;
		return -1;
	}

	ret = pico_socket_close(s);

	/* Delete the fd_elem struct associated with the file descriptor */ 
	fd_elem_delete(fd);

	return ret;
}
