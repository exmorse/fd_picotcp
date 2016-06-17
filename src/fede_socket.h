#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

ssize_t pico_read(int fd, void* buf, size_t len);

ssize_t pico_write(int fd, const void* buf, size_t count);

int pico_listen(int fd, int backlog);

int pico_close(int fd);

int pico_socket(int domain, int type, int protocol);

int pico_bind(int fd, const struct sockaddr* addr, socklen_t addrlen);

int pico_connect (int fd, const struct sockaddr* addr, socklen_t addrlen);

int pico_accept (int fd, struct sockaddr* addr, socklen_t* addrlen); 
