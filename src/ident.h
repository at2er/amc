/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef AMC_IDENT_H
#define AMC_IDENT_H
#include "object.h"
#include "type.h"
#include "str.h"
#include <stdbool.h>

struct yz_ident {
	bool is_func_arg;
	bool mut;
	str name;
	struct yz_object *object;
	struct mcb_operand operand;
	struct yz_type type;
};

void drop_ident(struct yz_ident *self);
void free_yz_ident(struct yz_ident *self);

#endif
