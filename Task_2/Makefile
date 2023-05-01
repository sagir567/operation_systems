FLAGS = -Wall -g
CC = gcc


all: stshell cmp copy codecB.so codecA.so encode decode



stshell.o: Part_c/stshell.c
	$(CC) $(FLAGS) -c stshell.c

stshell: Part_c/stshell.o
	$(CC) $(FLAGS) -o Part_c/stshell Part_c/stshell.o

cmp.o: Part_a/cmp.c
	$(CC) $(FLAGS) -c Part_a/cmp.c

copy.o: Part_a/copy.c
	$(CC) $(FLAGS) -c Part_a/copy.c

cmp: Part_a/cmp.o
	$(CC) $(FLAGS) -o Part_a/cmp Part_a/cmp.o

copy: Part_a/copy.o
	$(CC) $(FLAGS) -o Part_a/copy Part_a/copy.o

codecB.o: Part_b/codecB.c
	${CC} -fPIC ${FLAGS} -c Part_b/codecB.c

codecA.o: Part_b/codecA.c
	${CC} -fPIC ${FLAGS} -c Part_b/codecA.c


codecA.so: Part_b/codecA.o
	$(CC) $(FLAGS) -shared Part_b/codecA.o -o Part_b/codecA.so

codecB.so: Part_b/codecB.o
	$(CC) $(FLAGS) -shared Part_b/codecB.o -o Part_b/codecB.so

encode.o: Part_b/encode.c
	$(CC) $(FLAGS) -c Part_b/encode.c

encode: Part_b/encode.o
	$(CC) $(FLAGS) -o Part_b/encode Part_b/encode.o

decode.o: Part_b/decode.c
	$(CC) $(FLAGS) -c Part_b/decode.c

decode: Part_b/decode.o
	$(CC) $(FLAGS) -o Part_b/decode Part_b/decode.o

.PHONEY: clean
clean:
	rm -f *.o *.a *.so stshell shell cmp copy decode encode