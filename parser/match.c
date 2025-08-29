/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "include/block.h"
#include "include/expr.h"
#include "include/indent.h"
#include "include/keywords.h"
#include "include/lexer.h"
#include "include/utils.h"
#include "../include/backend.h"
#include "../include/enum.h"
#include "../include/match.h"
#include "../include/parser.h"
#include "../utils/utils.h"
#include <stdio.h>

struct match_context_enum {
	yz_enum *self;
	unsigned int enum_count;
};

union match_context_data {
	struct match_context_enum e;
	void *v;
};

struct match_context {
	union match_context_data data;
	backend_cond_match_handle *handle;
	enum MATCH_MODE mode;
	struct parser *parser;
};

static int match_end(struct match_context *context);
static int match_end_enum_mode(struct match_context *context);
static enum MATCH_MODE match_get_mode(yz_val *val);
static int match_handle_case_context(struct match_context *context,
		yz_val *val);
static int match_handle_case_enum(struct match_context *context, yz_val *val);
static int match_handle_context(struct match_context *context, yz_val *val);
static int match_handle_enum_mode(struct match_context *context, yz_val *val);
static int match_parse_body(struct match_context *context, yz_val *val);
static enum LEXER_RESULT match_parse_case(struct match_context *context);
static yz_val *match_parse_case_cond(struct parser *parser);
static yz_val *match_parse_condition(struct parser *parser);

int match_end(struct match_context *context)
{
	switch (context->mode) {
	case MATCH_MODE_COND: return 0; break;
	case MATCH_MODE_ENUM:
		return match_end_enum_mode(context);
		break;
	default: break;
	}
	return 1;
}

int match_end_enum_mode(struct match_context *context)
{
	if (context->data.e.enum_count != context->data.e.self->count)
		goto err_must_match_all;
	return 0;
err_must_match_all:
	printf(LEXER_ERR_FMT ERROR_STR":\n"
			"| You must match all cases "
			"of the enum: '%s'\n",
			LEXER_ERR_FMT_ARG(context->parser->lexer),
			context->data.e.self->name.s);
	return 1;
}

enum MATCH_MODE match_get_mode(yz_val *val)
{
	if (val->type.type != AMC_SYM)
		return MATCH_MODE_FAULT;
	if (val->data.sym->result_type.type == YZ_ENUM)
		return MATCH_MODE_ENUM;
	return MATCH_MODE_FAULT;
}

int match_handle_case_context(struct match_context *context, yz_val *val)
{
	switch (context->mode) {
	case MATCH_MODE_COND: return 0; break;
	case MATCH_MODE_ENUM:
		return match_handle_case_enum(context, val);
		break;
	default: break;
	}
	return 1;
}

int match_handle_case_enum(struct match_context *context, yz_val *val)
{
	if (val->type.type != YZ_ENUM_ITEM)
		goto err_not_enum;
	if (val->type.v != context->data.e.self)
		goto err_not_same_enum;
	context->data.e.enum_count += 1;
	return 0;
err_not_enum:
	printf(LEXER_ERR_FMT ERROR_STR":\n"
			"| Match enum but case not enum!\n",
			LEXER_ERR_FMT_ARG(context->parser->lexer));
	return 1;
err_not_same_enum:
	printf(LEXER_ERR_FMT ERROR_STR":\n"
			"| Match enum and case enum is different!\n",
			LEXER_ERR_FMT_ARG(context->parser->lexer));
	return 1;
}

int match_handle_context(struct match_context *context, yz_val *val)
{
	if ((context->mode = match_get_mode(val)) == MATCH_MODE_FAULT)
		return 1;
	switch (context->mode) {
	case MATCH_MODE_COND: return 0; break;
	case MATCH_MODE_ENUM:
		return match_handle_enum_mode(context, val);
		break;
	default: break;
	}
	return 1;
}

int match_handle_enum_mode(struct match_context *context, yz_val *val)
{
	if (val->type.type != AMC_SYM)
		return 1;
	if (val->data.sym->result_type.type != YZ_ENUM)
		return 1;
	context->data.e.enum_count = 0;
	context->data.e.self = val->data.sym->result_type.v;
	return 0;
}

int match_parse_body(struct match_context *context, yz_val *val)
{
	enum LEXER_RESULT ret = 0;
	context->handle = backend_call(cond_match_begin)(context->mode);
	if (context->handle == NULL)
		return 1;
	free_yz_val(val);
	while ((ret = match_parse_case(context))
			!= LEXER_RESULT_END) {
		if (ret == LEXER_RESULT_FAULT)
			goto err_free_handle;
	}
	if (match_end(context))
		goto err_free_handle;
	if (backend_call(cond_match_end)(context->handle))
		return 1;
	return 0;
err_free_handle:
	backend_call(cond_match_free_handle)(context->handle);
	return 1;
}

enum LEXER_RESULT
match_parse_case(struct match_context *context)
{
	struct sclexer lexer_record;
	uint64_t orig_pos = sclexer_record(&context->parser->lexer,
			&lexer_record);
	struct lexer_tok tok;
	yz_val *val = NULL;
	if (orig_pos == -1)
		return LEXER_RESULT_FAULT;
	if (sclexer_get_line(&context->parser->lexer) == EOF)
		return LEXER_RESULT_FAULT;
	if (indent_read(&context->parser->lexer)
			!= context->parser->scope->indent)
		goto restore_end;
	if (lexer_read_tok(&tok, &context->parser->lexer))
		return LEXER_RESULT_FAULT;
	if (tok.type != TOK_TYPE_PIPE_LINE)
		goto restore_end;
	if ((val = match_parse_case_cond(context->parser)) == NULL)
		return LEXER_RESULT_FAULT;
	if (match_handle_case_context(context, val))
		return LEXER_RESULT_FAULT;
	if (backend_call(cond_match_case)(context->handle, val))
		return LEXER_RESULT_FAULT;
	if (parse_block(context->parser))
		return LEXER_RESULT_FAULT;
	if (backend_call(cond_match_case_end)(context->handle))
		return LEXER_RESULT_FAULT;
	return LEXER_RESULT_CONTINUE;
restore_end:
	if (sclexer_restore(&context->parser->lexer, &lexer_record, orig_pos))
		return LEXER_RESULT_FAULT;
	return LEXER_RESULT_END;
}

yz_val *match_parse_case_cond(struct parser *parser)
{
	struct expr *expr = NULL;
	yz_val *val = NULL;
	i64 orig_column = parser->lexer.column,
	    orig_line = parser->lexer.line;
	if ((expr = parse_expr(parser, 1)) == NULL)
		goto err_cannot_parse_expr;
	if (expr_apply(parser, expr) > 0)
		goto err_cannot_apply_expr;
	if ((val = expr2yz_val(expr)) == NULL)
		goto err_free_expr;
	return val;
err_cannot_parse_expr:
	err_print_pos(__func__, "Cannot parse expression!",
			orig_line, orig_column);
	backend_stop(BE_STOP_SIGNAL_ERR);
	return NULL;
err_cannot_apply_expr:
	err_print_pos(__func__, "Cannot apply expression!",
			orig_line, orig_column);
	backend_stop(BE_STOP_SIGNAL_ERR);
err_free_expr:
	free_expr(expr);
	return NULL;
}

yz_val *match_parse_condition(struct parser *parser)
{
	struct expr *expr = NULL;
	i64 orig_column = parser->lexer.column,
	    orig_line = parser->lexer.line;
	if ((expr = parse_expr(parser, 1)) == NULL)
		goto err_cannot_parse_expr;
	if (expr_apply(parser, expr) > 0)
		goto err_cannot_apply_expr;
	if (lexer_try_next_line(&parser->lexer) != TRY_RESULT_HANDLED)
		goto err_no_nl;
	return expr2yz_val(expr);
err_cannot_parse_expr:
	err_print_pos(__func__, "Cannot parse expression!",
			orig_line, orig_column);
	backend_stop(BE_STOP_SIGNAL_ERR);
	return NULL;
err_cannot_apply_expr:
	err_print_pos(__func__, "Cannot apply expression!",
			orig_line, orig_column);
	backend_stop(BE_STOP_SIGNAL_ERR);
	return NULL;
err_no_nl:
	free_expr(expr);
	printf(LEXER_ERR_FMT"Missing line break character.\n",
			LEXER_ERR_FMT_ARG(parser->lexer));
	return NULL;
}

int parse_match(struct parser *parser)
{
	yz_val *val = NULL;
	struct match_context context = {
		.data.v = NULL,
		.parser = parser
	};
	if (parser->lexer.cur[0] == '\n') {
		context.mode = MATCH_MODE_COND;
		return match_parse_body(&context, NULL);
	}
	if ((val = match_parse_condition(context.parser)) == NULL)
		return 1;
	if (match_handle_context(&context, val))
		goto err_free_val;
	if (match_parse_body(&context, val))
		goto err_free_val;
	return 0;
err_free_val:
	free_yz_val(val);
	return 1;
}
