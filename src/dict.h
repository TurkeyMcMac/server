#ifndef DICT_INCLUDED_
#define DICT_INCLUDED_

#include <stddef.h>
#include <stdint.h>

#define DICT_KEY_MAX_LENGTH 8

union dict_key {
	int64_t bits;
	char str[8];
};

struct dict_entry {
	union dict_key key;
	void *val;
};

struct dict {
	size_t size, cap;
	struct dict_entry *entries;
};

int dict_init(struct dict *di, size_t cap);

int dict_add(struct dict *di, const char *key, void *val);

int dict_freeze(struct dict *di);

void *dict_get(const struct dict *di, const char *key);

void dict_destroy(struct dict *di);

#endif /* DICT_INCLUDED_ */
