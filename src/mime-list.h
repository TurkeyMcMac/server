#ifndef MIME_LIST_INCLUDED_
#define MIME_LIST_INCLUDED_

#include "dict.h"

struct mime_list {
	struct dict file_ends;
	char *strbuf;
	size_t buf_cap;
	size_t buf_used;
};

int mime_list_read(struct mime_list *list, const char *path);

void mime_list_destroy(struct mime_list *list);

#endif /* MIME_LIST_INCLUDED_ */
