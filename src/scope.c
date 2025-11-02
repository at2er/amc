/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "scope.h"
#include "symbol.h"
#include <stddef.h>
#include <stdlib.h>

#define CAPACITY_SHIFT 8

void append_symbol_to_scope(struct yz_symbol *self, struct yz_scope *scope)
{
	scope->symbols_count++;
	if (scope->symbols_capacity < scope->symbols_count) {
		scope->symbols_capacity += CAPACITY_SHIFT;
		scope->symbols = realloc(scope->symbols,
				sizeof(*scope->symbols)
				* scope->symbols_capacity);
	}
	scope->symbols[scope->symbols_count - 1] = self;
}

struct yz_scope *create_yz_scope(struct yz_scope *parent)
{
	struct yz_scope *result = calloc(1, sizeof(*result));
	result->parent = parent;
	return result;
}

struct yz_scope *drop_scope(struct yz_scope *self)
{
	struct yz_scope *parent = self->parent;
	for (size_t i = 0; i < self->symbols_count; i++)
		drop_symbol(self->symbols[i]);
	free_yz_scope(self);
	return parent;
}

void free_yz_scope(struct yz_scope *self)
{
	free(self->symbols);
	free(self);
}
