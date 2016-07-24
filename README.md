fd_picoTCP
==========

Fd_picotcp is a wrapper for picotcp that allows to manipulate picotcp socket (linked to 
vde, tap or tun devices) using the standard socket interface, and perform poll() and 
select() simultaneously on both picotcp sockets and kernel sockets.

Requirements:
-------------

- 	vde2
- 	libvdeplug-dev
- 	picotcp (Included)


Compiling fd_picotcp:
---------------------

- move to src/picotcp 
- run: 	
 	```
	make posix core ARCH=shared
	
	make test ARCH=shared
	```
- move back to src/ and run:

	```
	make
	```
- optionally it is possible to run (with root privilege):
	```
	make install
	```
  to move libraries and header files to standard locations


Writing programs using fd_picotcp:
---------------------------------

- include ```<fd_picotcp.h>```
- create fd_picotcp sockets using ```socket()```, with  ```AF_PICO_INET``` or ```AF_PICO_INET6```
  as first argument
  * If ```socket()``` is called with any other first argument the ```socket()``` system
    call is executed
- now it is possible to perform the standard socket operations on the file 
  descriptor returned by ```socket()```

####Customize Device  
Before creating the first socket, it is possible to customize the environment.
These operations are not necessary, since there are default values for the corresponding parameters.

- ```set_device_type(int type)```, where ```DEVICE_VDE```, ```DEVICE_TAP``` and ```DEVICE_TUN``` are the possible values, and ```DEVICE_VDE``` the default one
- ```set_device_mac(unsigned char mac[6])```, where the default is a randomly generated MAC
- ```set_switch_path(char* path)```, with ```/tmp/vde.ctl``` as the default value 
- ```set_interface_name(char* name)```, to set tap or tun interface name
- ```set_ipv4_netmask(char* new_mask)```, with the mask given in string format (e.g. ```255.255.255.0```)
- ```set_ipv6_netmask(char* new_mask)```, with the mask given in string format (e.g. ```ffff::```)   



Compiling programs using fd_picotcp:
-----------------------------------
####Compiling with the static ```libfdpicotcp.a```:
Run gcc with the following flags:

	-I PATH/TO/FD_PICOTCP/src -L PATH/TO/FD_PICOTCP/static_lib -lfdpicotcp

	-ldl -lpthread -lvdeplug
	
- There's no need to use ```-I``` and ```-L``` flags if the library is installed (```make install```)
	
####Compiling with the shared ```libfdpicotcp.so```:
Run gcc with the following flags:

	-I PATH/TO/FD_PICOTCP/src -L PATH/TO/FD_PICOTCP/shared_lib -lfdpicotcp

	-ldl -lpthread -lvdeplug

then add the path to the shared object to ```LD_LIBRARY_PATH``` before launching executables:

	  LD_LIBRARY_PATH=./PATH/TO/FDPICOTCP/shared_lib

	  export LD_LIBRARY_PATH


- There's no need to use ```-I``` and ```-L``` flags if the library is installed (```make install```)

Examples:
---------
Very basic application showing how to write and compile a program with fd_picotcp.

- move to examples/ directory
- run ```make``` to compile the examples 
	* the default Makefile uses the static libfdpicotcp.a library; to compile 
  	  with the shared libfdpicotcp.so use 
	  
	  ```make -f Makefile_so```
	
	  then, if the library is not installed, run:
	  ```	
	  LD_LIBRARY_PATH=./PATH/TO/FDPICOTCP/shared_lib

	  export LD_LIBRARY_PATH
	  ```
		
- launch a VDE switch in a new terminal with the command ```vde_switch```
- launch the server:
	```
	./fd_server <IPv4_Address> <Port>
	example: 	./fd_server 10.0.0.1 5000
	```	

- launch the client:
	```
	./fd_client <IPv4_Address> <IPv4_Server_Address> <Server_Port>
	example:	./fd_client 10.0.0.2 10.0.0.1 5000
	```
	
Now the standard input of fd_client is sent to fd_server using fd_picotcp sockets.
	
fd_server6 and fd_client6 are the same application, using IPv6.
	
