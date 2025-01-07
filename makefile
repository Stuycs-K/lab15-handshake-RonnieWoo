compile: persistant_client.o forking_server.o pipe_networking.o 
	@gcc -o persistant_client persistant_client.o pipe_networking.o
	@gcc -o forking_server forking_server.o pipe_networking.o

client: persistant_client
	@./persistant_client

server: forking_server
	@./forking_server

persistant_client.o: persistant_client.c pipe_networking.h
	@gcc -c persistant_client.c

forking_server.o: forking_server.c pipe_networking.h
	@gcc -c forking_server.c

pipe_networking.o: pipe_networking.c pipe_networking.h
	@gcc -c pipe_networking.c

clean:
	rm *.o persistant_client forking_server *.fifo
	rm *~