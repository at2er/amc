/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef AMC_COMPILER_LET_H
#define AMC_COMPILER_LET_H
#include "../expr.h"
#include "../ident.h"
#include <mcb/mcb.h>

void compile_let(struct mcb_context *mcb,
		struct yz_ident *ident,
		const struct yz_expr *expr);

#endif
