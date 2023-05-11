FLAGS = -Wall -g
CC = gcc


all: stshell cmp copy codecB.so codecA.so encode decode



stshell.o: stshell.c
	$(CC) $(FLAGS) -c stshell.c

stshell: stshell.o
	$(CC) $(FLAGS) -o stshell stshell.o

cmp.o: cmp.c
	$(CC) $(FLAGS) -c cmp.c

copy.o: copy.c
	$(CC) $(FLAGS) -c copy.c

cmp: cmp.o
	$(CC) $(FLAGS) -o cmp cmp.o

copy: copy.o
	$(CC) $(FLAGS) -o copy copy.o

codecB.o: codecB.c
	${CC} -fPIC ${FLAGS} -c codecB.c

codecA.o: codecA.c
	${CC} -fPIC ${FLAGS} -c codecA.c


codecA.so: Part_b/codecA.o
	$(CC) $(FLAGS) -shared codecA.o -o codecA.so

codecB.so: Part_b/codecB.o
	$(CC) $(FLAGS) -shared codecB.o -o codecB.so

encode.o: Part_b/encode.c
	$(CC) $(FLAGS) -c encode.c

encode: Part_b/encode.o
	$(CC) $(FLAGS) -o encode encode.o

decode.o: Part_b/decode.c
	$(CC) $(FLAGS) -c decode.c

decode: Part_b/decode.o
	$(CC) $(FLAGS) -o decode decode.o

.PHONEY: clean
clean:
	rm -f *.o *.a *.so stshell shell cmp copy decode encode
