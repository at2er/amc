/**
 * Please Comply with open source license.
 * @license: MIT License
 * @source: https://github.com/at2er/libsctrie
 */

#ifndef __LIBSCTRIE_H
#define __LIBSCTRIE_H
#include <limits.h>
#include <stdlib.h>

struct sctrie__tree_node_t;
struct sctrie__tree_node_t {
	struct sctrie__tree_node_t *nodes[UCHAR_MAX + 1];
	void *data;
};

extern void *sctrie_find_elem(const void *tree, const char *str, int str_len);
extern void *sctrie_append_elem(void *tree, size_t node_size,
		const char *str, int str_len);
extern void *sctrie_append_or_find_elem(void *tree, size_t node_size,
		const char *str, int str_len);
extern void sctrie_free_node(void *tree, void (*node_free)(void *node));
extern void sctrie_free_tree(void *tree, void (*node_free)(void *node));
extern void sctrie_free_tree_noself(void *tree, void (*node_free)(void *node));

#endif
