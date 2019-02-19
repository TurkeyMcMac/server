#ifndef FILE_INFO_INCLUDED_
#define FILE_INFO_INCLUDED_

#include "mime-list.h"

struct file_info {
	const char *type;
	const char *encoding;
	long size;
};

#endif /* FILE_INFO_INCLUDED_ */
