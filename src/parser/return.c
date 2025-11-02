/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "expr.h"
#include "return.h"
#include "../compiler/return.h"
#include "../type.h"
#include <assert.h>

int parse_return(struct parser *parser)
{
	struct yz_expr *expr;

	assert(parser->cur_func);

	expr = parse_expr(parser);
	if (!expr)
		return 1;

	if (!convert_type_implicity(&parser->cur_func->type, expr->sum_type))
		goto err_implicit_convertion_type;
	expr->sum_type = &parser->cur_func->type;

	compile_return(&parser->mcb, expr);
	return 0;
err_implicit_convertion_type:
	printf(ERR_FMT"unsupport implicit convertions type '%s' and '%s'\n",
			ERR_FMT_ARG(parser),
			type_get_str(parser->cur_func->type.type),
			type_get_str(expr->sum_type->type));
	return 1;
}
