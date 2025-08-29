/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "include/constructor.h"

void free_constructor_handle(struct constructor_handle *self)
{
	free_constructor_handle_no_self(self);
	free(self);
}

void free_constructor_handle_no_self(struct constructor_handle *self)
{
	for (int i = 0; i < self->len; i++) {
		if (self->vs[i] == NULL)
			return;
		free_yz_val(self->vs[i]);
	}
	free(self->vs);
}
