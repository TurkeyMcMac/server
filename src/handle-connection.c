#include "handle-connection.h"
#include "make-header.h"
#include "escape-path.h"
#include <fcntl.h>
#include <string.h>

#define BUF_SIZE 2048
#define PRELUDE_SIZE 4
#define MAX_PATH_SIZE 1024

void handle_connection(int connfd, int rootfd)
{
	int file;
	char buf[BUF_SIZE];
	char *path;
	int status;
	int size;
	if ((size = read(connfd, buf, BUF_SIZE)) < 0) return;
	buf[size] = '\0';
	if (strncmp("GET ", buf, PRELUDE_SIZE)) {
		status = 501;
		goto send_just_header;
	}
	path = buf + PRELUDE_SIZE;
	path = strchr(path, ' ');
	if (path) {
		*path = '\0';
	} else {
		status = 400;
		goto send_just_header;
	}
	path = buf + PRELUDE_SIZE;
	size = escape_path(path, path);
	if (size < 0) {
		status = -size;
		goto send_just_header;
	}
	if (size > MAX_PATH_SIZE) {
		status = 501;
		goto send_just_header;
	}
	--path;
	path[0] = '.';
	if ((file = openat(rootfd, path, O_RDONLY)) < 0) {
		status = 404;
		goto send_just_header;
	}
	status = 200;
	write(connfd, buf, make_header(status, buf, BUF_SIZE));
	do {
		size = read(file, buf, BUF_SIZE);
		write(connfd, buf, size);
	} while (size == BUF_SIZE);
	return;

send_just_header:
	write(connfd, buf, make_header(status, buf, BUF_SIZE));
}