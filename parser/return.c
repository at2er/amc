/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "compiler/return.h"
#include "literal.h"
#include "parser/expr.h"
#include "parser/return.h"

int parse_return(struct parser *parser)
{
	struct yz_expr *expr = parse_expr(parser);
	if (!expr)
		return 1;
	compile_return(parser->mcb, expr);
	return 0;
}
