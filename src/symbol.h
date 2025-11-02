/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef AMC_SYMBOL_H
#define AMC_SYMBOL_H
#include "str.h"
#include <limits.h>
#include <stddef.h>

enum YZ_SYMBOL_TYPE {
	YZ_SYM_NOT_USE,

	YZ_ENUM,
	YZ_FUNC,
	YZ_FUNC_ARG,
	YZ_IDENT,
	YZ_STRUCT,
	YZ_SYMBOL_TYPE_COUNT
};

union yz_symbol_data {
	struct yz_enum   *yz_enum;
	struct yz_func   *yz_func;
	struct yz_ident  *yz_func_arg;
	struct yz_ident  *yz_ident;
	struct yz_struct *yz_struct;
};

struct yz_symbol {
	struct yz_symbol *nodes[UCHAR_MAX + 1];

	union yz_symbol_data data;
	enum YZ_SYMBOL_TYPE type;
};

struct yz_symbol *append_symbol(struct yz_symbol *tree, str *name);
void drop_symbol(struct yz_symbol *self);

#endif
