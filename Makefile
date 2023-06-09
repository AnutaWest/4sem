CC=cc

CFLAGS=-std=gnu11 -c

all: main

main: main.o my_lib.o
lab3.o: main.c
	$(CC) $(CFLAGS) main.c

my_lib.o: my_lib.c
	$(CC) $(CFLAGS) my_lib.c


clean:
	rm -rf *.o main