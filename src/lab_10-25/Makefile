PROGS= palindrome reverse-file

all: $(PROGS)

palindrome: palindrome.o
	$(CC) -o $@ $^

reverse-file: reverse-file.o
	$(CC) -o $@ $^

clean: rm -f *.o a.out $(PROGS)

include ../Makefile.common
