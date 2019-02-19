#include "handle-connection.h"
#include "make-header.h"
#include "escape-path.h"
#include "open-file.h"
#include <fcntl.h>
#include <string.h>

#define BUF_SIZE 2048
#define PRELUDE_SIZE 4
#define MAX_PATH_SIZE 1024

void handle_connection(int connfd, const struct shared *share)
{
	struct file_info finfo;
	int file;
	char buf[BUF_SIZE];
	char *path;
	int status;
	int size;
	struct stat st;
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
	if ((file = open_file(share->rootfd, path, &finfo, &share->mime)) < 0) {
		status = -file;
		goto send_just_header;
	}
	status = 200;
	write(connfd, buf, make_header(status, buf, BUF_SIZE, &finfo));
	do {
		if ((size = read(file, buf, BUF_SIZE)) < 0) return;
		write(connfd, buf, size);
	} while (size == BUF_SIZE);
	return;

send_just_header:
	write(connfd, buf, make_header(status, buf, BUF_SIZE, NULL));
}
