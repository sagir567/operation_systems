FLAGS = -Wall -g
CC = gcc


all:  codecA.so encode  codecB.so decode


codecB.o: codecB.c 
	${CC} -fPIC ${FLAGS} -c codecB.c 

codecA.o: codecA.c 
	${CC} -fPIC ${FLAGS} -c codecA.c 


codecA.so: codecA.o 
	$(CC) $(FLAGS) -shared codecA.o -o codecA.so

codecB.so: codecB.o
	$(CC) $(FLAGS) -shared codecB.o -o codecB.so	

encode.o: encode.c 
	$(CC) $(FLAGS) -c encode.c 

encode: encode.o
	$(CC) $(FLAGS) -o encode encode.o

decode.o: decode.c 
	$(CC) $(FLAGS) -c decode.c 

decode: decode.o
	$(CC) $(FLAGS) -o decode decode.o
	
	
.PHONEY: clean
clean:
	rm -f *.o *.a *.so decode encode 
