/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "fn.h"
#include "ident.h"
#include "literal.h"
#include "type.h"
#include "utils.h"
#include <stdint.h>
#include <stdio.h>

void free_yz_func(struct yz_func *self)
{
	if (!self)
		return;
	if (self->name.s != self->path.s)
		str_free_noself(&self->path);
	str_free_noself(&self->name);
	free_yz_type_noself(&self->type);
	for (uint16_t i = 0; i < self->argc; i++)
		free_yz_ident(self->args[i]);
}

void free_yz_func_call(struct yz_func_call *self)
{
	if (!self)
		return;
}

void print_yz_func_call(const struct yz_func_call *self, uint16_t depth)
{
	printf("%*s\x1b[33mcall "
			"\x1b[32m%s\x1b[0m "
			"(\x1b[32m%s\x1b[0m) "
			"\x1b[33m{\x1b[0m\n",
			depth, "",
			self->callee->name.s,
			self->callee->path.s);
	depth += PRINT_SHIFT;
	for (uint16_t i = 0; i < self->argc; i++)
		print_yz_literal(self->args[i], depth);
	depth -= PRINT_SHIFT;
	printf("%*s\x1b[33m}\x1b[0m\n", depth, "");
}
