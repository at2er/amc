/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "expr.h"
#include "literal.h"
#include "type.h"

void free_yz_literal(struct yz_literal *self)
{
	if (!self)
		return;
	free_yz_type_noself(&self->type);
	//TODO free_yz_expr(self->data.expr);
}
