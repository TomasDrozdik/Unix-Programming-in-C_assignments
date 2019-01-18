PROGS= kill-workers

all: $(PROGS)

kill-workers: kill-workers.o
	$(CC) -pthread -o $@ $^

clean:
	rm -f *.o a.out $(PROGS)

include ../Makefile.common
