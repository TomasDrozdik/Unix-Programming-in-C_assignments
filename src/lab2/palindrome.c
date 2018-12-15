/* Palindrome detection */

#include <err.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int
check_palidrome(int fd)
{
	int i;
	off_t length;
	char c1, c2;

	if ((length = lseek(fd, 0, SEEK_END)) == -1) {
		err(1, "lseek");
	}

	// To remove the eof char.
	length -= 2;

	i = 0;

	while (i < length - i) {
		if (lseek(fd, i, SEEK_SET) == -1) {
			err(1, "lseek");
		}

		if (read(fd, &c1, 1) == -1) {
			err(1, "read");
		}

		if (lseek(fd, length - i, SEEK_SET) == -1) {
			err(1, "lseek");
		}

		if (read(fd, &c2, 1) == -1) {
			err(1, "read");
		}

		if (c1 != c2) {
			return (-1);
		}

		++i;
	}

	return (0);
}

int
main(int argc, char *argv[])
{
	int fd;

	if (argc != 2) {
		errx(1, "argc");
	}

	if ((fd = open(argv[1], O_RDONLY)) == -1) {
		err(1, "open");
	}

	printf("File %s is a palindorme: %s", argv[1],
		check_palidrome(fd) == 0 ? "TRUE" : "FALSE");

	return (0);
}