FLAGS = -Wall -g
CC = gcc


all: cmp 



cmp.o: cmp.c 
	$(CC) $(FLAGS) -c cmp.c 



cmp: cmp.o
	$(CC) $(FLAGS) -o cmp cmp.o


.PHONEY: clean
clean:
	# rm -f *.o *.a *.so cmp copy
