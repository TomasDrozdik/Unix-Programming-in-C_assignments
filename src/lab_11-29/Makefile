PROGS= bdoor client-select client-threads

all: $(PROGS)

bdoor: bdoor.o 
	$(CC) -o $@ $^

client-select: client-select.o 
	$(CC) -o $@ $^

client-threads: client-threads.o 
	$(CC) -o $@ $^ -pthread

clean:
	rm -f *.o a.out $(PROGS)

include ../Makefile.common
