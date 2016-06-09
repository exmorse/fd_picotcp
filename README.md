fd_picoTCP
==========

Fd_picotcp is a wrapper for picotcp that allows to manipulate picotcp socket (linked to 
vde, tap or tun devices) using the standard socket interface, and perform poll() and 
select() simultaneously on picotcp sockets and kernel sockets.

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


Writing program using fd_picotcp:
---------------------------------

- include ```<fd_picotcp.h>```

- customize device (vde, tap or tun) and environment using ```set_device_type()```, ```set_device_mac()```,
  ```set_interface_name()```, ```set_switch_path()```,  ```set_ipv4_netmask()```, and ```set_ipv6_netmask()``` 
  before creating any fd_picotcp  sockets 
  (NOT NECESSARY, default values are set)

- create fd_picotcp sockets using ```socket()```, with  ```AF_PICO_INET``` or ```AF_PICO_INET6```
  as first argument
  * If ```socket()``` is called with any other first argument the ```socket()``` system
    call is executed

- now it is possible to perform the standard socket operations on the file 
  descriptor returned by ```socket()```


Compiling program using fd_picotcp:
-----------------------------------
Run gcc with the following flags:

	-I PATH/TO/FD_PICOTCP/src -L PATH/TO/FD_PICOTCP/src 

	-ldl -lpthread -lfdpicotcp


Examples:
---------
Very basic application showing how to write and compile a program with fd_picotcp.

- move to examples/ directory
- run ```make``` to compile the examples 
	* the default Makefile uses the static libfdpicotcp.a library, to compile 
  	  with the shared libfdpicotcp.so use 
	  
	  ```make -f Makefile_so```
	
	  then run:
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
	./fd_client <IPv4_Address> <IPv4_Server_address> <Server_Port>
	example:	./fd_client 10.0.0.2 10.0.0.1 5000
	```
	
Now the standard input of fd_client is sent to fd_server using fd_picotcp sockets.
	
fd_server6 and fd_client6 are the same application, using IPv6.
	
