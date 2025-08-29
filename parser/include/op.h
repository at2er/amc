/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef AMC_PARSER_OP_H
#define AMC_PARSER_OP_H
#include "../../include/expr.h"
#include "../../include/symbol.h"

int op_apply_special(struct parser *parser, struct expr *e);
int op_assign(struct parser *parser, struct expr *e);
struct expr *op_extract_val_expr_create(yz_type *sum_type,
		yz_extract_val *val);

#endif
