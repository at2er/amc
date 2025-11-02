/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef AMC_COMPILER_IDENT_H
#define AMC_COMPILER_IDENT_H
#include "../ident.h"
#include <mcb/mcb.h>

void compile_ident_literal(struct mcb_context *mcb,
		const struct yz_ident *ident);

#endif
