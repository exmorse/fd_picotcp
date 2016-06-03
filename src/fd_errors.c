/**
	fd_errors.c

	This file contains the functions used to handle errors.
		
**/

#define _GNU_SOURCE

#include "fd_picotcp.h"
#include "fd_extern.h"

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

void fd_perror() {
	printf("fd_perror: ");
	switch (fd_err) {
		case FD_OK:
			printf("OK\n");
			break;
		
		case FD_SOCKET_NOT_FOUND:
			printf("Socket not found\n");
			break;
		
		case FD_SOCKET_IS_NULL:
			printf("Socket is null\n");
			break;
			
		case FD_ELEM_NOT_ALLOCATED:
			printf("Error allocating fd element\n");
			break;
		
		case FD_SEM_NOT_ALLOCATED:
			printf("Error allocating semaphore\n");
			break;
		
		case FD_TABLE_WRITE_ON_NOT_NULL:
			printf("Writing on not-null element\n");
			break;
		
		case FD_TABLE_MAX_SIZE_REACHED:
			printf("Max number of file descriptor reached\n");
			break;
		
		case FD_INVALID_ADDRESS:
			printf("Invalid address\n");
			break;
		
		case FD_INVALID_NETMASK:
			printf("Invalid netmask\n");
			break;
		
		case FD_PIPE_NOT_INITIALIZED:
			printf("Error initializing pipe\n");
			break;
		
		case FD_INTERFACE_NOT_LINKED:
			printf("Error linking picotcp interface\n");
			break;
		
		case FD_TABLE_INSERT_ERROR:
			printf("Error inserting elemet in table\n");
			break;
		
		case FD_THREAD_ERROR:
			printf("Error launching init thread\n");
			break;
		
		default:
			return;
	} 
}
