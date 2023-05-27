
FLAGS = -Wall -g
CC = gcc

all: react_server libreact_server.so

react_server.o: react_server.c 
	$(CC) $(FLAGS) -fPIC -c react_server.c 

react_server: react_server.o
	$(CC) $(FLAGS) -o react_server react_server.o

libreact_server.so: react_server.o
	$(CC) -shared -o libreact_server.so react_server.o

.PHONEY: clean
clean:
	rm -f *.o *.a *.so react_server
