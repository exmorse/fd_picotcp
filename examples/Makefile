INCLUDE_FD_PICOTCP=-I ../src -L ../src -lfdpicotcp
INCLUDE_PICOTCP=-I ../src/picotcp/build/include/ -L ../src/picotcp/build/lib 
LINK_FLAGS=-ldl -lvdeplug -lpthread

all: fd_server fd_client fd_server6 fd_client6 

fd_server: fd_server.c  
	$(CC) $(CFLAGS) -rdynamic -o fd_server fd_server.c $(INCLUDE_FD_PICOTCP) $(INCLUDE_PICOTCP) $(LINK_FLAGS) 

fd_client: fd_client.c  
	$(CC) $(CFLAGS) -rdynamic -o fd_client fd_client.c $(INCLUDE_FD_PICOTCP) $(INCLUDE_PICOTCP) $(LINK_FLAGS) 

fd_server6: fd_server6.c  
	$(CC) $(CFLAGS) -rdynamic -o fd_server6 fd_server6.c $(INCLUDE_FD_PICOTCP) $(INCLUDE_PICOTCP) $(LINK_FLAGS) 

fd_client6: fd_client.c  
	$(CC) $(CFLAGS) -rdynamic -o fd_client6 fd_client6.c $(INCLUDE_FD_PICOTCP) $(INCLUDE_PICOTCP) $(LINK_FLAGS) 

clean:
	rm fd_server fd_client fd_server6 fd_client6
