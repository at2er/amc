/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef AMC_MODULE_H
#define AMC_MODULE_H
#include "symbol.h"

struct yz_module {
	struct yz_symbol symbols;
};

void yz_module_init(struct yz_module *self);

#endif
