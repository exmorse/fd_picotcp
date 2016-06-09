ssize_t read(int fd, void* buf, size_t len);

ssize_t write(int fd, const void* buf, size_t count);

int listen(int fd, int backlog);

int close(int fd);

int socket(int domain, int type, int protocol);

int bind(int fd, const struct sockaddr* addr, socklen_t addrlen);

int connect (int fd, const struct sockaddr* addr, socklen_t addrlen);

int accept (int fd, struct sockaddr* addr, socklen_t* addrlen); 
