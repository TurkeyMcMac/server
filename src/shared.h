#ifndef SHARED_INCLUDED_
#define SHARED_INCLUDED_

#include "mime-list.h"

struct shared {
	struct mime_list mime;
	int rootfd;
	int sockfd;
	int logfd; /* Currently unused */
};

#endif /* SHARED_INCLUDED_ */
