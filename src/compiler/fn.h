/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef AMC_COMPILER_FN_H
#define AMC_COMPILER_FN_H
#include "../fn.h"
#include <mcb/expr.h>
#include <mcb/mcb.h>
#include <mcb/operand.h>

void build_mcb_expr_func_call_operand(struct mcb_expr_operand *result,
		const struct yz_func_call *self);
void compile_func_call(struct mcb_context *mcb,
		struct mcb_operand *result,
		const struct yz_func_call *self);
void compile_func_def(struct mcb_context *mcb, const struct yz_func *self);
void compile_func_def_end(struct mcb_context *mcb);

#endif
