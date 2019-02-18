#include "dict.h"
#include <stdlib.h>
#include <string.h>

typedef union dict_key KEY;
typedef struct dict_entry ENTRY;

static void key_from_str(KEY *key, const char *str)
{
	key->bits = 0;
	strncpy(key->str, str, DICT_KEY_MAX_LENGTH);
}

#define KEY_EQ(k1, k2) ((k1).bits == (k2).bits)

static int compare_keys(const void *e1, const void *e2)
{
	int64_t bits1 = ((const ENTRY *)e1)->key.bits,
		bits2 = ((const ENTRY *)e2)->key.bits;
	if (bits1 < bits2) return -1;
	if (bits1 > bits2) return 1;
	return 0;
}

int dict_init(struct dict *di, size_t cap)
{
	if (cap < 8) cap = 8;
	if (!(di->entries = malloc(cap * sizeof(*di->entries)))) return -1;
	di->cap = cap;
	di->size = 0;
	return 0;
}

int dict_add(struct dict *di, const char *key, void *val)
{
	size_t i;
	KEY k;
	ENTRY *ent;
	key_from_str(&k, key);
	for (i = 0; i < di->size; ++i) {
		/* TODO: something better than linear search */
		if (KEY_EQ(k, di->entries[i].key)) return 0;
	}
	if (di->size >= di->cap) {
		size_t new_cap;
		ENTRY *new_entries;
		new_cap = di->cap * 3 / 2;
		new_entries =
			realloc(di->entries, new_cap * sizeof(*di->entries));
		if (!new_entries) return -1;
		di->entries = new_entries;
		di->cap = new_cap;
	}
	ent = &di->entries[di->size];
	ent->key = k;
	ent->val = val;
	++di->size;
	return 1;
}

int dict_freeze(struct dict *di)
{
	ENTRY *new_entries;
	new_entries = realloc(di->entries, di->size * sizeof(*di->entries));
	if (!new_entries) return -1;
	di->entries = new_entries;
	di->cap = di->size;
	qsort(di->entries, di->size, sizeof(*di->entries), compare_keys);
}

void *dict_get(const struct dict *di, const char *key)
{
	const ENTRY *found;
	size_t min, max;
	KEY k;
	if (di->size == 0) return NULL;
	key_from_str(&k, key);
	min = 0;
	max = di->size;
	while (min + 1 < max) {
		size_t mid = (min + max) / 2;
		if (k.bits - di->entries[mid].key.bits < 0) {
			max = mid;
		} else {
			min = mid;
		}
	}
	found = &di->entries[min];
	if (KEY_EQ(found->key, k)) return found->val;
	return NULL;
}

void dict_destroy(struct dict *di)
{
	free(di->entries);
}
