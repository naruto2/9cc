CFLAGS=-std=c11 -g -static -fno-common -Wall
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

9cc: $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

$(OBJS): 9cc.h

test: 9cc
	./9cc tests > tmp.s
	echo 'int ext1; int *ext2; int char_fn() { return 257; }' \
		'int static_fn() { return 5; }' | \
		gcc -xc -c -o tmp2.o -
	gcc -static -o tmp tmp.s tmp2.o
	./tmp

queen: 9cc
	./9cc examples/nqueen.c > tmp.s
	gcc -static -o tmp tmp.s
	./tmp > tmp.txt
	gcc examples/nqueen.c -o tmpo -Wno-builtin-declaration-mismatch
	./tmpo > tmpo.txt
	diff tmp.txt tmpo.txt

clean:
	rm -f 9cc *.o *~ tmp*

.PHONY: test clean
