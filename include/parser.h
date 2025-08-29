/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef AMC_PARSER_H
#define AMC_PARSER_H
#include "decorator.h"
#include "module.h"
#include "scope.h"
#include <limits.h>
#include <sclexer.h>

struct parsed_node {
	struct parsed_node *nodes[UCHAR_MAX];
	struct scope *scope;
};

struct parsed_list {
	struct parsed_node *nodes[UCHAR_MAX];
};

struct parser_imported_node {
	struct parser_imported_node *nodes[UCHAR_MAX];
	yz_module *mod;
};

struct parser_imported {
	struct parser_imported_node *nodes[UCHAR_MAX];
	int count;
	yz_module **mods;
};

/**
 * Restore this struct after keyword parsed.
 */
struct parser_stat {
	unsigned int has_pub:1;
	struct decorators decorators;
};

struct parser {
	struct parser_imported imported;
	struct sclexer lexer;
	str path;
	struct scope *scope;
	struct scope *scope_pub;
	struct parser_stat stat;
	struct symbol *sym;
	str target;
};

struct global_parser {
	unsigned int has_err:1, has_main:1;
	struct parsed_list parsed;
	str output, root_dir, root_mod, target_path;
};

extern struct global_parser global_parser;

struct parser *parse_file(str *path, const char *real_path);
struct parser *parser_create(str *path, const char *real_path);
int parser_get_target_from_mod_path(str *result, str *path);
int parser_imported_append(struct parser_imported *imported, yz_module *mod);
yz_module *parser_imported_find(struct parser_imported *imported, str *name);
int parser_init(const char *path);
struct scope *parser_parsed_file_find(str *path);
int parser_stat_restore(struct parser_stat *self);

void free_parser(struct parser *self);
void free_parser_imported(struct parser_imported *self);
void free_parser_imported_mods(int count, yz_module **mods);
void free_parser_imported_nodes(struct parser_imported_node *root,
		int free_mod);
void free_parser_stat_noself(struct parser_stat *self);

#endif
