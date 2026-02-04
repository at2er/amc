#include "sctrie.h"
#include <stdint.h>
#include <stdlib.h>

void *sctrie_find_elem(const void *tree, const char *str, int str_len)
{
	struct sctrie__tree_node_t *result = (struct sctrie__tree_node_t*)tree;
	for (int i = 0; i < str_len; i++) {
		if (result->nodes[(uint8_t)str[i]] == NULL)
			return NULL;
		result = result->nodes[(uint8_t)str[i]];
	}
	return result;
}

void *sctrie_append_elem(void *tree, size_t node_size,
		const char *str, int str_len)
{
	struct sctrie__tree_node_t *cur = (struct sctrie__tree_node_t*)tree;
	for (int i = 0; i < str_len; i++) {
		if (cur->nodes[(uint8_t)str[i]] != NULL) {
			if (i + 1 == str_len)
				return NULL;
			cur = cur->nodes[(uint8_t)str[i]];
			continue;
		}
		cur->nodes[(uint8_t)str[i]] = calloc(1, node_size);
		cur = cur->nodes[(uint8_t)str[i]];
	}
	return cur;
}

void *sctrie_append_or_find_elem(void *tree, size_t node_size,
		const char *str, int str_len)
{
	struct sctrie__tree_node_t *cur = (struct sctrie__tree_node_t*)tree;
	for (int i = 0; i < str_len; i++) {
		if (cur->nodes[(uint8_t)str[i]] == NULL)
			cur->nodes[(uint8_t)str[i]] = calloc(1, node_size);
		cur = cur->nodes[(uint8_t)str[i]];
	}
	return cur;
}

void sctrie_free_node(void *tree, void (*node_free)(void *node))
{
	struct sctrie__tree_node_t *cur = (struct sctrie__tree_node_t*)tree;
	for (int i = 0; i < UCHAR_MAX; i++) {
		if (cur->nodes[i] == NULL)
			continue;
		sctrie_free_node(cur->nodes[i], node_free);
	}
	node_free(cur);
}

void sctrie_free_tree_noself(void *tree, void (*node_free)(void *node))
{
	struct sctrie__tree_node_t *cur = (struct sctrie__tree_node_t*)tree;
	for (int i = 0; i < UCHAR_MAX; i++) {
		if (cur->nodes[i] == NULL)
			continue;
		sctrie_free_node(cur->nodes[i], node_free);
	}
}

void sctrie_free_tree(void *tree, void (*node_free)(void *node))
{
	sctrie_free_tree_noself(tree, node_free);
	node_free(tree);
}
