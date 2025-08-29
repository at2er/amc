/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
//TODO: Mutable array
#include "include/array.h"
#include "include/constructor.h"
#include "include/expr.h"
#include "include/identifier.h"
#include "include/lexer.h"
#include "include/op.h"
#include "include/type.h"
#include "include/utils.h"
#include "../include/array.h"
#include "../include/backend.h"
#include "../include/parser.h"
#include <sclexer.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int array_get_elem_handle_val(yz_val *val, yz_val *offset,
		struct symbol *sym);
static int array_get_len(struct sclexer *lexer, yz_array_type *arr);
static yz_val *array_read_offset(struct parser *f);
static int constructor_array_elem(struct constructor_handle *handle);

int array_get_elem_handle_val(yz_val *val, yz_val *offset, struct symbol *sym)
{
	yz_array_type *arr = sym->result_type.v;
	yz_extract_val *v = malloc(sizeof(*v));
	v->sym = sym;
	v->elem = sym;
	v->data.offset = offset;
	v->type = YZ_EXTRACT_ARRAY;
	if ((val->data.v = op_extract_val_expr_create(&arr->type, v)) == NULL)
		return 1;
	val->type.type = AMC_EXPR;
	val->type.v = val->data.v;
	return 0;
}

int array_get_len(struct sclexer *lexer, yz_array_type *arr)
{
	struct lexer_tok tok;
	arr->len = 0;
	if (lexer_read_tok(&tok, lexer) || tok.type != TOK_TYPE_SCLEXER)
		return 1;
	if (tok.self.type == SCLEXER_TOK_TYPE_INT_NEG)
		goto err_neg_len;
	if (tok.self.type != SCLEXER_TOK_TYPE_INT)
		goto err_not_number;
	if (lexer_read_tok(&tok, lexer) || tok.type != TOK_TYPE_BRACKET_R)
		return 1;
	return 0;
err_neg_len:
	printf(LEXER_ERR_FMT"Array len is negtive number\n",
			LEXER_ERR_FMT_ARG_REF(lexer));
	return 1;
err_not_number:
	printf(LEXER_ERR_FMT"Array len not a integer\n",
			LEXER_ERR_FMT_ARG_REF(lexer));
	return 1;
}

yz_val *array_read_offset(struct parser *parser)
{
	yz_type type = {.type = YZ_U64, .v = NULL};
	struct expr *expr = NULL;
	i64 orig_column = parser->lexer.column,
	    orig_line = parser->lexer.line;
	if ((expr = parse_expr(parser, 1)) == NULL)
		goto err_read_offset_failed;
	if (expr_apply(parser, expr) > 0)
		goto err_read_offset_failed;
	return identifier_handle_expr_val(expr, &type);
err_read_offset_failed:
	printf(LEXER_ERR_FMT"Read offset failed.\n",
			__func__, parser->lexer.fpath,
			orig_line, orig_column);
	return NULL;
}

int constructor_array_elem(struct constructor_handle *handle)
{
	yz_array_type *arr = handle->sym->result_type.v;
	struct expr *expr = NULL;
	yz_val *val = NULL;
	if (handle->index > handle->len - 1)
		goto err_too_many_elem;
	if ((expr = parse_expr(handle->parser, 1)) == NULL)
		goto err_cannot_parse_expr;
	if (expr_apply(handle->parser, expr) > 0)
		goto err_cannot_apply_expr;
	if ((val = identifier_handle_expr_val(expr, &arr->type)) == NULL)
		goto err_cannot_apply_expr;
	handle->vs[handle->index] = val;
	handle->index += 1;
	return 0;
err_too_many_elem:
	printf(LEXER_ERR_FMT"Too many elements!\n",
			LEXER_ERR_FMT_ARG(handle->parser->lexer));
	backend_stop(BE_STOP_SIGNAL_ERR);
	return 1;
err_cannot_parse_expr:
	printf(LEXER_ERR_FMT"Cannot parse expr!\n",
			LEXER_ERR_FMT_ARG(handle->parser->lexer));
	backend_stop(BE_STOP_SIGNAL_ERR);
	return 1;
err_cannot_apply_expr:
	printf(LEXER_ERR_FMT"Cannot apply expr!\n",
			LEXER_ERR_FMT_ARG(handle->parser->lexer));
	backend_stop(BE_STOP_SIGNAL_ERR);
	return 1;
}

int array_get_elem(struct parser *parser, yz_val *val)
{
	struct symbol *sym = val->data.v;
	yz_val *offset = NULL;
	if (sym->result_type.type != YZ_ARRAY)
		goto err_not_arr;
	if ((offset = array_read_offset(parser)) == NULL)
		return 1;
	return array_get_elem_handle_val(val, offset, sym);
err_not_arr:
	printf("amc: array_get_elem: %lu,%lu: Symbol: '%s' isn't array!\n",
			parser->lexer.line, parser->lexer.column,
			sym->name.s);
	backend_stop(BE_STOP_SIGNAL_ERR);
	return 1;
}

int array_set_elem(struct parser *parser, struct symbol *sym, yz_val *offset,
		enum OP_ID mode)
{
	yz_array_type *arr = sym->result_type.v;
	i64 orig_column = parser->lexer.column,
	    orig_line = parser->lexer.line;
	yz_val *val = NULL;
	if (sym->result_type.type != YZ_ARRAY)
		return err_print_pos(__func__, NULL,
				parser->lexer.line, parser->lexer.column);
	if (identifier_assign_get_val(parser, &arr->type, &val))
		return 1;
	if (backend_call(array_set_elem)(sym, offset, val, mode))
		return err_print_pos(__func__, "Backend call failed!",
				orig_line, orig_column);
	free_yz_val(val);
	return 0;
}

int constructor_array(struct parser *parser, struct symbol *sym)
{
	struct constructor_handle handle = {
		.index = 0,
		.len = ((yz_array_type*)sym->result_type.v)->len,
		.parser = parser,
		.sym = sym,
		.vs = calloc(handle.len, sizeof(yz_val*))
	};
	int ret = 0;
	while ((ret = constructor_array_elem(&handle)) != LEXER_RESULT_END) {
		if (ret == LEXER_RESULT_FAULT)
			goto err_free_handle;
	}
	if (backend_call(array_def)(&sym->backend_status,
				handle.vs, handle.len))
		goto err_backend_failed;
	free_constructor_handle_no_self(&handle);
	return 0;
err_backend_failed:
	free_constructor_handle_no_self(&handle);
	printf(LEXER_ERR_FMT"Backend call failed!\n",
			LEXER_ERR_FMT_ARG(parser->lexer));
	backend_stop(BE_STOP_SIGNAL_ERR);
err_free_handle:
	free_constructor_handle_no_self(&handle);
	return 1;
}

int parse_type_array(struct parser *parser, yz_type *type)
{
	yz_array_type *arr = NULL;
	int ret = 0;
	struct lexer_tok tok;
	arr = malloc(sizeof(*arr));
	arr->len = -1;
	type->type = YZ_ARRAY;
	type->v = arr;
	if ((ret = parse_type(parser, &arr->type)) > 0)
		goto err_free_arr;
	if (lexer_read_tok(&tok, &parser->lexer)
			|| tok.type != TOK_TYPE_COMMA)
		goto err_free_arr;
	if (array_get_len(&parser->lexer, arr))
		goto err_free_arr;
	return 0;
err_free_arr:
	free(arr);
	type->v = NULL;
	type->type = AMC_ERR_TYPE;
	return 1;
}
