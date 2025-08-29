/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef AMC_SCOPE_H
#define AMC_SCOPE_H
#include "backend/scope.h"
#include "symbol.h"

struct scope {
	struct symbol *fn;
	int indent;
	struct scope *parent;

	backend_scope_status *status;

	struct yz_user_type *types[UCHAR_MAX + 1];

	struct symbol_group sym_groups[SYM_GROUPS_SIZE];
};

int scope_check_is_correct(struct scope *scope);
int scope_end(struct scope *scope);

void free_scope(struct scope *scope);
void free_scope_noself(struct scope *scope);

#endif
