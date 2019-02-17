#include "serve.h"
#include "handle-connection.h"
#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SIG_END SIGUSR1

union sockaddr_u {
	struct sockaddr_in inet;
	struct sockaddr gen;
};

static int start_worker(int sockfd, int rootfd)
{
	sigset_t block;
	sigfillset(&block);
	sigdelset(&block, SIGSEGV);
	sigdelset(&block, SIGBUS);
	sigdelset(&block, SIGFPE);
	sigdelset(&block, SIGILL);
	while (1) {
		sigset_t old;
		int connfd;
		if ((connfd = accept(sockfd, NULL, NULL)) < 0) {
			return -errno;
		}
		sigprocmask(SIG_BLOCK, &block, &old);
		handle_connection(connfd, rootfd);
		sigprocmask(SIG_SETMASK, &old, NULL);
		close(connfd);
	}
	return 0;
}

static int start_serving(int sockfd, const struct serve_info *info)
{
	sigset_t oldset;
	int i;
	int err;
	void (*old_end_sig)(int);
	pid_t pgroup = getpgrp();
	for (i = 0; i < info->n_workers; ++i) {
		pid_t worker = fork();
		if (worker) {
			if (worker >= 0) {
				setpgid(worker, pgroup);
			} else {
				err = -errno;
				goto end_all;
			}
		} else {
			_exit(start_worker(sockfd, info->rootfd));
		}
	}
	sigprocmask(SIG_BLOCK, &info->catch, &oldset);
	sigwaitinfo(&info->catch, NULL);
	sigprocmask(SIG_SETMASK, &oldset, NULL);
	err = 0;
end_all:
	old_end_sig = signal(SIG_END, SIG_IGN);
	killpg(pgroup, SIG_END);
	signal(SIG_END, old_end_sig);
	errno = 0;
	do {
		wait(NULL); /* TODO: handle exit status */
	} while (errno != ECHILD);
	close(sockfd);
	return err;
}

int serve(const struct serve_info *info)
{
	int sockfd;
	union sockaddr_u addr;
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) return -errno;
	addr.inet.sin_family = AF_INET;
	addr.inet.sin_port = htons(info->port);
	addr.inet.sin_addr = info->addr;
	if (bind(sockfd, &addr.gen, sizeof(addr.inet))) return -errno;
	if (listen(sockfd, info->backlog)) return -errno;
	if (start_serving(sockfd, info)) return -errno;
}
