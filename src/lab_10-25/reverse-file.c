/* Reverse file */

#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

#define MMAP_SIZE 2

void
reverse_by_bytes(int infd, int outfd)
{
	int length, i;
	char c;

	if ((length = lseek(infd, 0, SEEK_END)) == -1) {
		err(1, "lseek");
	}

	// To remove EOF and newline.
	i = 2;
	while (i <= length) {
		if (lseek(infd, length - i, SEEK_SET) == -1) {
			err(1, "lseek");
		}

		if (read(infd, &c, 1) == -1) {
			err(1, "read");
		}

		if (write(outfd, &c, 1) == -1) {
			err(1, "write");
		}

		++i;
	}
}

void
reverse_mmap(int infd, int outfd)
{
	int length, mmapsize, readlength, i;
	char *addr;

	if ((length = lseek(infd, 0, SEEK_END)) == -1) {
		err(1, "lseek");
	}

	length -= 2;

	readlength = 0;
	while (readlength <= length) {
		mmapsize = (length - readlength < MMAP_SIZE) ?
			length - readlength : MMAP_SIZE;

		if ((addr = mmap(0, mmapsize, PROT_READ, MAP_PRIVATE,
			infd, length - mmapsize)) == MAP_FAILED) {
			err(1, "mmap");
		}

		readlength += mmapsize;

		i = mmapsize;
		while (i > 0) {
			write(outfd, addr + i, 1);
			--i;
		}

		munmap(addr, mmapsize);
	}
}

void
usage(void)
{
	printf("./a.out <infile> <outfile>");
}

int
main(int argc, char *argv[])
{
	int infd, outfd;
	mode_t mode = S_IRUSR | S_IWUSR | S_IWGRP | S_IROTH;

	if (argc != 3) {
		usage();
		return (1);
	}

	if ((infd = open(argv[1], O_RDONLY)) == -1) {
		err(1, "open infile");
	}

	if ((outfd = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC, mode)) == -1) {
		err(1, "open outfile");
	}

//	reverse_by_bytes(infd, outfd);

	reverse_mmap(infd, outfd);

	return (0);
}