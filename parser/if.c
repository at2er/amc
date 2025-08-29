/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "include/block.h"
#include "include/expr.h"
#include "include/indent.h"
#include "include/keywords.h"
#include "../include/backend.h"
#include "../include/parser.h"
#include "include/lexer.h"
#include <stdio.h>

struct if_context {
	backend_cond_if_handle *handle;
	struct parser *parser;
};

static int if_condition_parse(struct parser *parser);
static enum LEXER_RESULT if_continue(struct if_context *context);
static int if_parse_block(struct parser *parser);
static int parse_else(struct if_context *context);

int if_condition_parse(struct parser *parser)
{
	struct expr *expr = NULL;
	uint64_t orig_column = parser->lexer.column,
	         orig_line = parser->lexer.line;
	if ((expr = parse_expr(parser, 1)) == NULL)
		goto err_cannot_parse_expr;
	if (expr_apply(parser, expr) > 0)
		goto err_cannot_apply_expr;
	free_expr(expr);
	return 0;
err_cannot_parse_expr:
	printf("|< "LEXER_ERR_FMT"Cannot parse expression!\n",
			__func__, parser->lexer.fpath,
			orig_line, orig_column);
	backend_stop(BE_STOP_SIGNAL_ERR);
	return 1;
err_cannot_apply_expr:
	printf("|< "LEXER_ERR_FMT"Cannot apply expression!\n",
			__func__, parser->lexer.fpath,
			orig_line, orig_column);
	backend_stop(BE_STOP_SIGNAL_ERR);
	return 1;
}

enum LEXER_RESULT if_continue(struct if_context *context)
{
	struct sclexer lexer_record;
	uint64_t orig_pos =
		sclexer_record(&context->parser->lexer, &lexer_record);
	if (orig_pos == -1)
		return LEXER_RESULT_FAULT;
	if (indent_read(&context->parser->lexer)
			!= context->parser->scope->indent)
		goto restore_end;
	if (lexer_try_read_str(&context->parser->lexer, "else", 4)
			!= TRY_RESULT_HANDLED)
		goto restore_end;
	if (parse_else(context))
		return LEXER_RESULT_FAULT;
	return LEXER_RESULT_END;
restore_end:
	if (sclexer_restore(&context->parser->lexer, &lexer_record, orig_pos))
		return LEXER_RESULT_FAULT;
	return LEXER_RESULT_END;
}

int if_parse_block(struct parser *parser)
{
	if (parse_block(parser))
		goto err_parse_block_failed;
	return 0;
err_parse_block_failed:
	printf(LEXER_ERR_FMT"Parse block failed!\n",
			LEXER_ERR_FMT_ARG(parser->lexer));
	return 1;
}

int parse_else(struct if_context *context)
{
	struct lexer_tok tok;
	if (lexer_read_tok(&tok, &context->parser->lexer)
			|| tok.type != TOK_TYPE_BLOCK_START)
		goto err_block_start_not_found;
	if (parse_block(context->parser))
		goto err_parse_block_failed;
	if (backend_call(cond_else)(context->handle))
		goto err_backend_failed;
	return 0;
err_block_start_not_found:
	printf(LEXER_ERR_FMT"Block start symbol not found!\n",
			LEXER_ERR_FMT_ARG(context->parser->lexer));
	return 1;
err_parse_block_failed:
	printf(LEXER_ERR_FMT"Parse block failed!\n",
			LEXER_ERR_FMT_ARG(context->parser->lexer));
	return 1;
err_backend_failed:
	printf(LEXER_ERR_FMT"Backend call failed!\n",
			LEXER_ERR_FMT_ARG(context->parser->lexer));
	return 1;
}

int parse_if(struct parser *parser)
{
	struct if_context context = {
		.handle = backend_call(cond_if_begin)(),
		.parser = parser
	};
	int ret = 0;
	if (context.handle == NULL)
		goto err_backend_failed;
	if (if_condition_parse(parser))
		goto err_free_handle;
	if (backend_call(cond_if_cond)(context.handle))
		return 1;
	if (if_parse_block(parser))
		goto err_free_handle;
	if (backend_call(cond_if)(context.handle))
		goto err_free_handle;
	while ((ret = if_continue(&context)) != LEXER_RESULT_END) {
		if (ret == LEXER_RESULT_FAULT)
			goto err_free_handle;
	}
	if (backend_call(cond_if_end)(context.handle))
		return 1;
	return 0;
err_backend_failed:
	printf(LEXER_ERR_FMT"Backend call failed!\n",
			LEXER_ERR_FMT_ARG(parser->lexer));
	return 1;
err_free_handle:
	backend_call(cond_if_free_handle)(context.handle);
	return 1;
}
