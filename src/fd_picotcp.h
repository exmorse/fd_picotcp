#include <sys/stat.h>
#include <semaphore.h>
#include <inttypes.h>
#include <libvdeplug.h>

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
