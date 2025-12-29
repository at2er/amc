/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "module.h"
#include <assert.h>
#include <memory.h>

void init_module(struct yz_module *self)
{
	assert(self);
	memset(self, 0, sizeof(*self));
}
