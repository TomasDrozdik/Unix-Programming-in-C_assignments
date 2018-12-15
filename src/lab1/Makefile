PROGS= hello-world penultimate pointer-to-self is-environ environ-creat

all: $(PROGS)

hello-world: hello-world.o
	$(CC) -o $@ $^

penultimate: penultimate.o 
	$(CC) -o $@ $^

pointer-to-self: pointer-to-self.o 
	$(CC) -o $@ $^

is-environ: is-environ.o 
	$(CC) -o $@ $^

environ-creat: environ-creat.o 
	$(CC) -o $@ $^

clean: rm -f *.o a.out $(PROGS)

include ../Makefile.common
