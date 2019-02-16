#include "serve.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
	int err;
	struct serve_info info;
	if (argc < 3) return 1;
	info.rootfd = open(argv[3], O_RDONLY | O_DIRECTORY);
	info.port = atoi(argv[2]);
	inet_aton(argv[1], &info.addr);
	info.backlog = 64;
	info.n_workers = 4;
	sigemptyset(&info.catch);
	sigaddset(&info.catch, SIGINT);
	if ((err = serve(&info))) {
		printf("error: %s\n", strerror(-err));
	}
}
