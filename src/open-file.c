#include "open-file.h"
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

int open_file(int rootfd, const char *path, struct file_info *info,
	const struct mime_list *types)
{
	struct stat st;
	const char *ending;
	int fd;
	if ((fd = openat(rootfd, path, O_RDONLY)) < 0) return -404;
	if (fstat(fd, &st)) return -500;
	if (S_ISDIR(st.st_mode)) {
		if ((fd = openat(fd, ".index.html", O_RDONLY)) < 0) return -404;
		if (fstat(fd, &st)) return -500;
		if (S_ISDIR(st.st_mode)) {
			errno = EISDIR;
			return -404;
		}
		info->type = "text/html";
		goto return_file;
	}
	ending = strrchr(path, '.');
	info->type = ending ? dict_get(&types->file_ends, ending + 1) :
		"application/octet-stream";
return_file:
	info->encoding = "identity";
	info->size = st.st_size;
	return fd;
}
