COMPILE_PICOTCP=picotcp/build/modules/pico_dev_vde.o
INCLUDE_FD_PICOTCP=-I ./ 
INCLUDE_PICOTCP=-I picotcp/build/include

all: libfdpicotcp.so libfdpicotcp.a
	rm *.o

libfdpicotcp.so: fd_picotcp.o device.o fd_functions.o socket.o fd_errors.o
	cp picotcp/build/lib/libpicotcp.a ./
	ar x libpicotcp.a 
	gcc -shared -o libfdpicotcp.so *.o -lvdeplug -ldl -lpthread 
	rm libpicotcp.a
	mkdir ../shared_lib
	mv libfdpicotcp.so ../shared_lib 

libfdpicotcp.a: fd_picotcp.o device.o fd_functions.o socket.o fd_errors.o
	cp picotcp/build/lib/libpicotcp.a ./
	ar x libpicotcp.a 
	#ar rcs libfdpicotcp.a *.o $(COMPILE_PICOTCP) picotcp/build/lib/libpicotcp.a
	ar rcs libfdpicotcp.a *.o 
	rm libpicotcp.a
	mkdir ../static_lib
	mv libfdpicotcp.a ../static_lib 

fd_picotcp.o: fd_picotcp.c fd_picotcp.h
	$(CC) $(CFLAGS) -c -fPIC fd_picotcp.c $(INCLUDE_PICOTCP) $(INCLUDE_FD_PICOTCP) 

device.o: device.c 
	$(CC) $(CFLAGS) -c -fPIC device.c $(INCLUDE_PICOTCP) $(INCLUDE_FD_PICOTCP) 

fd_functions.o: fd_functions.c
	$(CC) $(CFLAGS) -c -fPIC fd_functions.c $(INCLUDE_PICOTCP) $(INCLUDE_FD_PICOTCP) 

socket.o: socket.c
	$(CC) $(CFLAGS) -c -fPIC socket.c $(INCLUDE_PICOTCP) $(INCLUDE_FD_PICOTCP) 

fd_errors.o: fd_errors.c
	$(CC) $(CFLAGS) -c -fPIC fd_errors.c $(INCLUDE_PICOTCP) $(INCLUDE_FD_PICOTCP) 

clean:
	rm -r ../static_lib ../shared_lib
