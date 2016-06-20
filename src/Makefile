INSTALL_PATH=/usr/local
INCLUDE_FD_PICOTCP=-I ./ 
INCLUDE_PICOTCP=-I picotcp/build/include

all: libfdpicotcp.so libfdpicotcp.a
	rm *.o

install: ../shared_lib/libfdpicotcp.so
	cp ../shared_lib/libfdpicotcp.so ${INSTALL_PATH}/lib
	cp fd_picotcp.h fede_socket.h ${INSTALL_PATH}/include

uninstall:
	rm ${INSTALL_PATH}/lib/libfdpicotcp.so
	rm ${INSTALL_PATH}/include/fd_picotcp.h ${INSTALL_PATH}/include/fede_socket.h

libfdpicotcp.so: fd_picotcp.o device.o fd_functions.o socket.o fd_errors.o fede_socket.o
	cp picotcp/build/lib/libpicotcp.a ./
	ar x libpicotcp.a 
	gcc -shared -o libfdpicotcp.so *.o -lvdeplug -ldl -lpthread 
	rm libpicotcp.a
	if [ ! -d ../shared_lib ]; then mkdir ../shared_lib; fi
	mv libfdpicotcp.so ../shared_lib 

libfdpicotcp.a: fd_picotcp.o device.o fd_functions.o socket.o fd_errors.o
	cp picotcp/build/lib/libpicotcp.a ./
	ar x libpicotcp.a 
	ar rcs libfdpicotcp.a *.o 
	rm libpicotcp.a
	if [ ! -d ..static_lib ]; then mkdir ../static_lib; fi
	mv libfdpicotcp.a ../static_lib 

fd_picotcp.o: fd_picotcp.c fd_picotcp.h
	$(CC) $(CFLAGS) -c -fPIC fd_picotcp.c $(INCLUDE_PICOTCP) $(INCLUDE_FD_PICOTCP) 

device.o: device.c 
	$(CC) $(CFLAGS) -c -fPIC device.c $(INCLUDE_PICOTCP) $(INCLUDE_FD_PICOTCP) 

fd_functions.o: fd_functions.c
	$(CC) $(CFLAGS) -c -fPIC fd_functions.c $(INCLUDE_PICOTCP) $(INCLUDE_FD_PICOTCP) 

socket.o: socket.c
	$(CC) $(CFLAGS) -c -fPIC socket.c $(INCLUDE_PICOTCP) $(INCLUDE_FD_PICOTCP) 

fede_socket.o: fede_socket.c
	$(CC) $(CFLAGS) -c -fPIC fede_socket.c $(INCLUDE_PICOTCP) $(INCLUDE_FD_PICOTCP) 

fd_errors.o: fd_errors.c
	$(CC) $(CFLAGS) -c -fPIC fd_errors.c $(INCLUDE_PICOTCP) $(INCLUDE_FD_PICOTCP) 

clean:
	if [ -d ../shared_lib ]; then rm -r ../shared_lib; fi
	if [ -d ../static_lib ]; then rm -r ../static_lib; fi
