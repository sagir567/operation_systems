.PHONY: all default clean

all: stnc

default: all

stnc: stnc.c
	gcc -o $@ $^ -lcrypto
	
clean:
	rm -f stnc
