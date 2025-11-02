/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef AMC_SCOPE_H
#define AMC_SCOPE_H
#include "symbol.h"
#include <limits.h>
#include <stddef.h>

enum YZ_SCOPE_TYPE {
	YZ_SCOPE_PRIVATE,
	YZ_SCOPE_PUBLIC,
	YZ_SCOPE_TYPE_COUNT
};

struct yz_scope {
	struct yz_scope *parent;
	struct yz_symbol **symbols;
	size_t symbols_count, symbols_capacity;
};

void append_symbol_to_scope(struct yz_symbol *self, struct yz_scope *scope);
struct yz_scope *create_yz_scope(struct yz_scope *parent);
struct yz_scope *drop_scope(struct yz_scope *self);
void free_yz_scope(struct yz_scope *self);

#endif
