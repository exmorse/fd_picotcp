/*
	fd_picotcp.c

	Main file of fd_picotcp.
	Contains functions used to initialize the picotvp and VDE environment 
	and manipulate core structures.
*/

#define _GNU_SOURCE

#include "fd_picotcp.h"
#include "fd_functions.h"
#include "fd_errors.h"

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

#include <pthread.h> 

#include <semaphore.h>

#include <sys/syscall.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/resource.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <time.h>

int FD_TABLE_SIZE = 8;
int MAX_FD;

/* Default VDE values */
char vde_switch_name[256] = "/tmp/vde.ctl";
unsigned char macaddr[6]={0x0, 0x0, 0x0, 0xa, 0xb, 0xc};
char device_name[8] = "vd";
char ipv4_mask_string[16] = "255.255.255.0";
char ipv6_mask_string[100] = "::";
int is_mac_changed = 0;
struct pico_device *pico_dev;

/* Type of picotcp device to create */
/* Set to VDE by default */
int device_type = DEVICE_VDE;

/* Array of pointer to fd_elem */
/* Used to store the active sockets */
fd_elem** fd_table;

/* Status of last fd_picotcp function called */
int fd_err = FD_OK;

/* Function to tick picotcp stack */
/* Used as a thread */
void* fd_pico_stack_loop() {
	while(1) {
		pico_stack_tick();
		usleep(2000);
	}
}

void _print_stats() {
	int i;
	for (i = 0; i < FD_TABLE_SIZE; i++) {
		if (fd_table[i] == NULL) printf("Pos: %d\t - NULL\n", i);
		else printf("Pos: %d\t - %d\n", i, (int)(fd_table[i]->fd[0]));
	}
}

void init() {
        struct pico_ip4 my_ip, netmask;
	int err, i;

	/* Allocate fd_elem dynamic array */
	fd_table = (fd_elem**) malloc(FD_TABLE_SIZE * sizeof(fd_elem*));

	/* Get max number of file descriptor per process */
	struct rlimit l;
	getrlimit(RLIMIT_NOFILE, &l);
	MAX_FD = l.rlim_cur;

	/* picotcp initializer */
	pico_stack_init();

	/* Set the fd_elem struct to NULL */
	for (i = 0; i < FD_TABLE_SIZE; i++) fd_table[i] = NULL;

	/* Modify MAC address only if it is still set to default value */
	/* No need to do it if user set a specific MAC */	
	if (!is_mac_changed) {
		macaddr[4] ^= (uint8_t)(getpid() >> 8); 
        	macaddr[5] ^= (uint8_t) (getpid() & 0xFF);
	}

	/* Set VDE device name to "vd-<PROCESS_PID>" */
	char pid_string[8];
	sprintf(pid_string, "%d", getpid());
	strcat(device_name, pid_string);

	/* Create picotcp device */
	switch (device_type) {
		case DEVICE_VDE:
        		pico_dev = (struct pico_device*) 
				pico_vde_create(vde_switch_name, device_name, macaddr);
			break;
	
		case DEVICE_TAP:
			printf("Trying to create device linked to %s\n", vde_switch_name);
			pico_dev = (struct pico_device*) pico_tap_create(vde_switch_name);
			break;
	
		case DEVICE_TUN:
			pico_dev = (struct pico_device*) pico_tun_create(vde_switch_name);
			break;
	
		default:
			return;
	}

	/* Launch thread to tick picotcp stack */
	pthread_t t;
	err = pthread_create(&t, NULL, fd_pico_stack_loop, NULL);	
	if (err == -1) {
		fd_err = FD_THREAD_ERROR;
	}
	return;
}

/* Callback function for every picotcp socket */
/* Handles "Connect" and "Read" events	*/
void handle_wakeup(uint16_t ev, struct pico_socket* socket) {
	
	/* Get fd_elem corresponding to picotcp socket that woke up */
	fd_elem* l = get_fd_elem_from_socket(socket);

	/* Connection on socket */
	if (ev & PICO_SOCK_EV_CONN) {
		if (l == NULL) {
			fd_err = FD_SOCKET_NOT_FOUND;
			return;
		}
		/* Unlock on accept semaphore */
		unlock_sem(l->accept_sem);
	}

	/* Something to read on socket */
	if (ev & PICO_SOCK_EV_RD) {
		if (l == NULL) {
			fd_err = FD_SOCKET_NOT_FOUND;
			return;
		}
		
		/* Write a character on fd_elem pipe */
		/* Any select or poll on this file descriptor will unlock */
		syscall(__NR_write, l->fd[1], "a", 1);
		l->fd_write_count++;
	
		/* Unlock on read semaphore */
		unlock_sem(l->read_sem);
	}
	
	if (ev & PICO_SOCK_EV_WR) {
	}
	if (ev & PICO_SOCK_EV_CLOSE) {
	}
}

/* Perform P on semaphore passed as argument */
void waiton_sem(sem_t* s) {
	sem_wait(s);
}

/* Perform V on semaphore passed as argument, only if its value is 0 */
/* That is because in case of multiple write() on the same socket only
   the first read() has to be non-blocking  */
void unlock_sem(sem_t* s) {
	int v = 0;
	sem_getvalue(s, &v);
	if (v > 0) return;
	sem_post(s);
}

/* Checks if string is a IPv4 address */
int isIpv4(char* address) {
	int i;
	for (i = 0; i < strlen(address); i++) {
		if (address[i] == '.')	return 1;
	}
	return 0;
}

/* Checks if string is a IPv6 address */
int isIpv6(char* address) {
	int i;
	for (i = 0; i < strlen(address); i++) {
		if (address[i] == ':')	return 1;
	}
	return 0;
}


/*-------------------------------------------------------------*/
/* The following are the function used to manipulate fd_table  */
/*-------------------------------------------------------------*/

int fd_elem_create(int fd[2], struct pico_socket* socket) {
	int e = 0;
	fd_elem* l = (fd_elem*)malloc(sizeof(fd_elem));
	
	if (l == NULL) {
		fd_err = FD_ELEM_NOT_ALLOCATED;
		return -1;
	}

	l->fd_write_count = 0;
	l->fd_char_count = 0;
	l->fd[0] = fd[0];
	l->fd[1] = fd[1];

	l->isIpv6 = 0;

	l->accept_sem = (sem_t*) malloc(sizeof(sem_t));
	l->read_sem = (sem_t*) malloc(sizeof(sem_t));

	if (l->accept_sem == NULL || l->read_sem == NULL) {
		fd_err = FD_SEM_NOT_ALLOCATED;
		return -1;
	}
	
	e = sem_init(l->accept_sem, 1, 0);
	if (e == -1) {
		perror("Sem1");
		return -1;
	}

	e = sem_init(l->read_sem, 1, 0);
	if (e == -1) {
		perror("Sem2");
		return -1;
	}
	
	l->socket = socket;

	if (fd[0] >= FD_TABLE_SIZE || fd[1] >= FD_TABLE_SIZE) {
		/* Try to duplicate table size */
		/* If table size has reached max then abort */
		if (2*FD_TABLE_SIZE > MAX_FD) {
			fd_err = FD_TABLE_MAX_SIZE_REACHED;
			return -1;
		}

		/* Else duplicate fd_table size */
		fd_elem** new_table = (fd_elem**) malloc(2*FD_TABLE_SIZE*sizeof(fd_elem*));
		if (new_table == NULL) {
			return -1;
		}
		int j;
		for (j = 0; j < FD_TABLE_SIZE; j++) new_table[j] = fd_table[j];		
		for (j = FD_TABLE_SIZE; j < 2*FD_TABLE_SIZE; j++) new_table[j] = NULL;
		FD_TABLE_SIZE = 2*FD_TABLE_SIZE;
		free(fd_table);
		fd_table = new_table;
	}

	if (fd_table[fd[0]] != NULL) {
		fd_err = FD_TABLE_WRITE_ON_NOT_NULL;
		return -1;
	}

	if (fd_table==NULL) printf("IS NULL\n");
	fd_table[fd[0]] = l;
	/* Returns the file descriptor */
	return fd[0];
}

int fd_elem_delete(int fd) {
	if (fd_table[fd] != NULL) {

		/* Close the pipe */
		close(fd_table[fd]->fd[0]);
		close(fd_table[fd]->fd[1]);

		/* Free memory and set to NULL */
		free(fd_table[fd]);
		fd_table[fd] = NULL;
		return 0;
	}
	return -1;
}

struct pico_socket* get_socket_from_fd (int fd) {
	if (fd_table[fd] == NULL) return NULL;
	return fd_table[fd]->socket;
}

fd_elem* get_fd_elem_from_fd (int fd) {
	return fd_table[fd];
}

fd_elem* get_fd_elem_from_socket (struct pico_socket* s) {
	int i;
	
	for (i = 0; i < FD_TABLE_SIZE; i++) {
		if (fd_table[i] != NULL && fd_table[i]->socket == s) {
			return fd_table[i];
		}
	}
	return NULL;
}

int is_socket_in_table (struct pico_socket* s) {
	int i;
	
	for (i = 0; i < FD_TABLE_SIZE; i++) {
		if (fd_table[i] != NULL && fd_table[i]->socket == s) {
			return i;
		}
	}
	return 0;
}
