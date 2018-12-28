/* tree.c */

/*
 *	Recursivly open and search all the arguments. If a dir_name is encountered
 * recursivly open it.
 */

#include <assert.h>
#include <err.h>
#include <errno.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void process_file(const char *);
void process_dir(const char *);
void print_dir(const char *);
void print_file(const char *);

// To get pretty printing
int level_of_recursion = 0;

void
process_file(const char *file)
{
	struct stat statbuf;
	mode_t mode;

	if (stat(file, &statbuf) == -1) {
		warn("stat");
	}
	mode = statbuf.st_mode;

	if (S_ISDIR(mode)) {
		process_dir(file);
	}
	else {
		print_file(file);
	}
}

void
print_file(const char *file)
{
	for (int i = 0; i < level_of_recursion; ++i) {
		printf("  ");
	}
	printf("\\->%s\n", file);
}

void
process_dir(const char *dir_name)
{
	DIR *dir;
	struct dirent *dir_item;

	print_dir(dir_name);

	if ((dir = opendir(dir_name)) == NULL) {
		warn("%s", dir_name);
		return;
	}

	if (chdir(dir_name) == -1) {
		warn("chdir");
		return;
	}

	++level_of_recursion;

	errno = 0;
	while ((dir_item = readdir(dir)) != NULL) {
		if (errno != 0) {
			err(-1, "readdir");
		}
		if (strcmp(dir_item->d_name, ".") != 0 &&
			strcmp(dir_item->d_name, "..") != 0) {

			process_file(dir_item->d_name);
		}
	}

	if (closedir(dir) == -1) {
		err(-1, "closedir");
	}

	if (chdir("..") == -1) {
		err(-1, "chdir");
	}

	--level_of_recursion;
}

void
print_dir(const char *file)
{
	for (int i = 0; i < level_of_recursion; ++i) {
		printf("  ");
	}
	printf("==%s==\n", file);
}

int
main(int argc, char *argv[])
{
	int i;

	if (argc < 2) {
		printf("usage: %s <filename>...", "a.out");
	}

	for (i = 1; i < argc; ++i) {
		process_file(argv[i]);
	}

}