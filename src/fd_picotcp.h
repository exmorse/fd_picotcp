#include <sys/stat.h>
#include <pico_stack.h>
#include <pico_dev_vde.h>
#include <pico_socket.h>
#include <pico_ipv6.h>
#include <semaphore.h>

#ifndef AF_PICO_INET 
	#define AF_PICO_INET 1018
#endif

#ifndef AF_PICO_INET6 
	#define AF_PICO_INET6 1019
#endif

#define FD_TABLE_SIZE 1024

/* Struct that associates a picotcp socket to a file descriptor 	*/
/* When a new fd_elem is created, fd[0] is returned, and will be used as
   key to identify the corresponding fd_elem 				*/
typedef struct s_fd_elem_t {
	int fd[2];		/* Pipe - select and poll will use this fd */     
	int fd_char_count; 	/* Number of char wrote and not read on fd */
	int isIpv6;		/* Boolean to determine if socket is IPv4 or IPv6 */
	sem_t* accept_sem;	/* Accept-Connect semaphore */
	sem_t* read_sem;	/* Read-Write semaphore */
	struct pico_socket* socket;	/* picotcp socket */
} fd_elem;


struct pico_socket* get_socket_from_fd(int);
fd_elem* get_fd_elem_from_fd(int);
fd_elem* get_fd_elem_from_socket(struct pico_socket*);

int fd_elem_create(int*, struct pico_socket*);
int fd_elem_delete(int fd);
int is_socket_in_table (struct pico_socket*);

void waiton_sem(sem_t*);
void unlock_sem(sem_t*);


int fd_pico_socket_open(uint16_t, uint16_t);
int fd_pico_socket_bind(int, char*, uint16_t);
int fd_pico_socket_listen(int, int);
int fd_pico_socket_connect(int, char*, uint16_t);
int fd_pico_socket_accept(int, char* ,int*);
int fd_pico_socket_write(int, const void*, int);
int fd_pico_socket_read(int, char*, int);
int fd_pico_socket_close(int);
