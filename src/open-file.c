#include "open-file.h"
#include <errno.h>
#include <fcntl.h>

int open_file(int rootfd, const char *path, struct stat *st)
{
	int fd;
	if ((fd = openat(rootfd, path, O_RDONLY)) < 0) return -404;
	if (fstat(fd, st)) return -500;
	if (S_ISDIR(st->st_mode)) {
		if ((fd = openat(fd, ".index.html", O_RDONLY)) < 0) return -404;
		if (fstat(fd, st)) return -500;
		if (S_ISDIR(st->st_mode)) {
			errno = EISDIR;
			return -404;
		}
		
	}
	return fd;
}
