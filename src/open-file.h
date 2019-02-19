#include "file-info.h"
#include "mime-list.h"

int open_file(int rootfd, const char *path, struct file_info *info,
        const struct mime_list *types);
