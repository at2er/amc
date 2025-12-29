/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "debug.h"
#include "ident.h"
#include "symbol.h"
#include "str.h"
#include <assert.h>
#include <sctrie.h>
#include <stdlib.h>

struct yz_symbol *append_symbol(struct yz_symbol *tree, str *name)
{
	struct yz_symbol *result = sctrie_append_or_find_elem(
			tree,
			sizeof(*result),
			STR_UNWRAP_REF(name));
	assert(result);
	if (result->type != YZ_SYM_NOT_USE) {
		debug_print("symbol is in use");
		return NULL;
	}
	return result;
}

void drop_symbol(struct yz_symbol *self)
{
	debug_print("drop symbol");
	if (self->type == YZ_IDENT)
		drop_ident(self->data.yz_ident);
	self->type = YZ_SYM_NOT_USE;
}
