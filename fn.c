/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "fn.h"
#include "ident.h"
#include "type.h"

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
