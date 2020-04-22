CFLAGS=-std=c11 -g -static -fno-common -Wall
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

9cc: $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

$(OBJS): 9cc.h

test: 9cc
	./9cc tests > tmp.s
	gcc -static -o tmp tmp.s
	./tmp

clean:
	rm -f 9cc *.o *~ tmp*

.PHONY: test clean
