#ifndef SERVER_INCLUDED_
#define SERVER_INCLUDED_

#include <signal.h>
#include <stdint.h>
#include <netinet/ip.h>

struct serve_info {
	int rootfd;
	int backlog;
	struct in_addr addr;
	uint16_t port;
	int n_workers;
	sigset_t catch;
};

int serve(const struct serve_info *info);

#endif /* SERVER_INCLUDED_ */
