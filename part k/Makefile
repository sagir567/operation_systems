FLAGS = -Wall -g
CC = gcc

all: stnc

stnc.o: stnc.c clientTCP.c
	$(CC) $(FLAGS) -c stnc.c 

stnc: stnc.o
	$(CC) $(FLAGS) -o stnc stnc.o

.PHONY: clean
clean:
	rm -f *.o *.a *.so stnc
