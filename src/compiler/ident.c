/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "ident.h"
#include "panic.h"
#include <mcb/mcb.h>
#include <mcb/operand.h>

void compile_ident_literal(struct mcb_context *mcb,
		struct mcb_operand *result,
		const struct yz_ident *ident)
{
	if (ident->is_func_arg) {
		*result = ident->operand;
		return;
	}
	if (MCB_CALL(mcb, gen_mov)(mcb, result, &ident->operand))
		PANIC_MCB_CALL;
}
