FLAGS = -Wall -g
CC = gcc


all: cmp copy

# cmp: cmp.o
# copy: copy.o

cmp.o: cmp.c 
	$(CC) $(FLAGS) -c cmp.c 

copy.o: copy.c 
	$(CC) $(FLAGS) -c copy.c 

cmp: cmp.o
	$(CC) $(FLAGS) -o cmp cmp.o

copy: copy.o
	$(CC) $(FLAGS) -o copy copy.o

.PHONEY: clean
clean:
	rm -f *.o *.a *.so cmp copy