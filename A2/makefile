CFLAGS = gcc -Wall -std=gnu99 -g

all: a2

a2: a2.o
	$(CFLAGS) -o a2 a2.o

a2.o: a2.c
	$(CFLAGS) -c a2.c

clean:
	rm *.o a2