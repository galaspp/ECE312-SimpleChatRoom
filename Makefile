all: server client

server: server.o
	gcc server.o -o server -pthread

client: client.o
	gcc client.o -o client

server.o: server.c
	gcc -c server.c -o server.o

client.o: client.c
	gcc -c client.c -o client.o

clean: 
	rm -f server client *.o *~