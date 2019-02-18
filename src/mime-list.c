#include "mime-list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *add_str(struct mime_list *list, const char *from, size_t size)
{
	char *str;
	size_t new_used = list->buf_used + size + 1;
	if (new_used >= list->buf_cap) return NULL; /* TODO: Growable buffer */
	str = list->strbuf + list->buf_used;
	memcpy(str, from, size);
	str[size] = '\0';
	list->buf_used = new_used;
	return str;
}

static void remove_str(struct mime_list *list, size_t size)
{
	++size;
	if (size > list->buf_used) {
		list->buf_used = 0;
	} else {
		list->buf_used -= size;
	}
}

int mime_list_read(struct mime_list *list, const char *path)
{
	FILE *file;
	size_t bufcap = 0;
	char *line = NULL;
	ssize_t linelen;
	list->buf_used = 0;
	list->buf_cap = 30000;
	if (!(list->strbuf = calloc(1, list->buf_cap))) goto error_strbuf;
	if (!(file = fopen(path, "r"))) goto error_fopen;
	if (dict_init(&list->file_ends, 100)) goto error_dict_init;
	while ((linelen = getline(&line, &bufcap, file)) >= 0) {
		char *reading;
		size_t token_len;
		char *typename, *ending;
		line[linelen] = '\n';
		if (line[0] == '\n' || line[0] == '#') continue;
		if (!(reading = memchr(line, '\t', linelen))) continue;
		token_len = (size_t)(reading - line);
		if (!(typename = add_str(list, line, token_len)))
			goto error_parsing;
		do {
			++reading;
		} while (*reading == '\t');
		if (*reading == '\n') {
			remove_str(list, token_len);
			continue;
		}
		do {
			char *this = reading;
			reading = strpbrk(reading, " \t\n");
			token_len = (size_t)(reading - this);
			if (!(ending = add_str(list, this, token_len)))
				goto error_parsing;
			if (dict_add(&list->file_ends, ending, typename) < 0)
				goto error_parsing;
		} while (*(reading++) != '\n');
	}
	if (!feof(file)) goto error_parsing;
	fclose(file);
	dict_freeze(&list->file_ends);
	return 0;

error_parsing:
	free(line);
	dict_destroy(&list->file_ends);
error_dict_init:
	fclose(file);
error_fopen:
	free(list->strbuf);
error_strbuf:
	return -1;
}

void mime_list_destroy(struct mime_list *list)
{
	free(list->strbuf);
	dict_destroy(&list->file_ends);
}
