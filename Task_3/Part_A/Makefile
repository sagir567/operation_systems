FLAGS = -Wall -g
CC = gcc


all: stnc

stnc.c: stnc.c ,clientTCP.c
	$(CC) $(FLAGS) -c stnc.c 

stnc.o: stnc.c 
	$(CC) $(FLAGS) -c stnc.c 

stnc: stnc.o
	$(CC) $(FLAGS) -o stnc stnc.o

clientTCP.o: clientTCP.c
	$(CC) $(FLAGS) -c clientTCP.c

clientTCP: clientTCP.o
	$(CC) $(FLAGS) -o clientTCP clientTCP.o

.PHONEY: clean
clean:
	rm -f *.o *.a *.so stnc