CC=gcc
CFLAGS=-Wall -Wextra -g
LDFLAGS=-pthread
DEPS=reactor.h
OBJ=main.o reactor.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

react_server: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(OBJ) react_server
