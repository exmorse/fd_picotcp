#include <sys/stat.h>
#include <semaphore.h>
#include <inttypes.h>
#include <libvdeplug.h>

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
