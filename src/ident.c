/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "ident.h"
#include "object.h"
#include "type.h"
#include <assert.h>

void drop_ident(struct yz_ident *self)
{
	assert(self);
	if (self->is_func_arg)
		return;
	if (!self->object)
		return;
	end_object_life(self->object);
	free_yz_object(self->object);
}

void free_yz_ident(struct yz_ident *self)
{
	if (!self)
		return;
	str_free_noself(&self->name);
	free_yz_type_noself(&self->type);
}
