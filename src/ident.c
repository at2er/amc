/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "ident.h"
#include "type.h"

void free_yz_ident(struct yz_ident *self)
{
	if (!self)
		return;
	str_free_noself(&self->name);
	free_yz_type_noself(&self->type);
}
