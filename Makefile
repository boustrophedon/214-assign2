CC=clang
CFLAGS=-Wall -Werror -g
LDFLAGS=-g

all: sl

sl: main.o libsl.a
		$(CC) $(LDFLAGS) main.o -L. -lsl -o sl

libsl.a: node.o sorted-list.o
		ar crvs libsl.a sorted-list.o node.o

main.o: main.c
		$(CC) $(CFLAGS) -c main.c

sorted-list.o: sorted-list.c
		$(CC) $(CFLAGS) -c sorted-list.c

node.o: node.c
		$(CC) $(CFLAGS) -c node.c

clean:
	rm main.o sorted-list.o node.o
	rm libsl.a
	rm sl
