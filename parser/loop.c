/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "include/expr.h"
#include "include/block.h"
#include "include/keywords.h"
#include "include/lexer.h"
#include "../include/backend.h"
#include "../include/parser.h"
#include <stdio.h>

static int loop_body_parse(struct parser *parser);
static int loop_condition_parse(struct parser *parser);

int loop_body_parse(struct parser *parser)
{
	if (parse_block(parser))
		goto err_parse_block_failed;
	return 0;
err_parse_block_failed:
	printf(LEXER_ERR_FMT"Parse block failed!\n",
			LEXER_ERR_FMT_ARG(parser->lexer));
	return 1;
}

int loop_condition_parse(struct parser *parser)
{
	struct expr *expr = NULL;
	if ((expr = parse_expr(parser, 1)) == NULL)
		goto err_parse_expr_failed;
	if (expr_apply(parser, expr) > 0)
		goto err_apply_expr_failed;
	free_expr(expr);
	return 0;
err_parse_expr_failed:
	printf(LEXER_ERR_FMT"Parse expression failed!\n",
			LEXER_ERR_FMT_ARG(parser->lexer));
	return 1;
err_apply_expr_failed:
	printf(LEXER_ERR_FMT"Apply expression failed!\n",
			LEXER_ERR_FMT_ARG(parser->lexer));
	return 1;
}

int parse_while(struct parser *parser)
{
	backend_while_handle *handle = backend_call(while_begin)();
	if (handle == NULL)
		goto err_backend_failed;
	if (loop_condition_parse(parser))
		goto err_free_handle;
	if (backend_call(while_cond)(handle))
		goto err_free_handle;
	if (loop_body_parse(parser))
		goto err_free_handle;
	if (backend_call(while_end)(handle))
		goto err_free_handle;
	return 0;
err_backend_failed:
	printf(LEXER_ERR_FMT"Backend call failed!\n",
			LEXER_ERR_FMT_ARG(parser->lexer));
err_free_handle:
	backend_call(while_free_handle)(handle);
	return 1;
}
