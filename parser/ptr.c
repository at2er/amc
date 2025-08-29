/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "include/identifier.h"
#include "include/lexer.h"
#include "include/ptr.h"
#include "include/type.h"
#include "include/utils.h"
#include "../include/backend.h"
#include "../include/parser.h"
#include "../include/ptr.h"
#include <stdlib.h>

static int parse_type_ptr_sub(yz_type *result);

int parse_type_ptr_sub(yz_type *result)
{
	yz_ptr_type *root = result->v,
	            *box = root->ref.v;
	root->level += box->level;
	root->ref.type = box->ref.type;
	root->ref.v = box->ref.v;
	box->ref.type = AMC_ERR_TYPE;
	box->ref.v = NULL;
	free(box);
	return 0;
}

int parse_type_ptr(struct parser *parser, yz_type *result)
{
	yz_ptr_type *box = NULL;
	struct lexer_tok tok;
	box = malloc(sizeof(yz_ptr_type));
	box->level = 1;
	if (lexer_try_read_str(&parser->lexer, "mut", 3)
			== TRY_RESULT_HANDLED)
		box->flag_mut = 1;
	result->type = YZ_PTR;
	result->v = box;
	if (parse_type(parser, &box->ref))
		return 1;
	if (box->ref.type == YZ_PTR && parse_type_ptr_sub(result))
		return 1;
	if (parser->lexer.cur[0] != '?')
		return 0;
	if (lexer_read_tok(&tok, &parser->lexer)
			|| tok.type != TOK_TYPE_QUESTION)
		return 1;
	box->flag_can_null = 1;
	return 0;
}

int ptr_set_val(struct parser *parser, struct symbol *ident, enum OP_ID mode)
{
	yz_val *val = NULL;
	yz_ptr_type *ptr = ident->result_type.v;
	if (ident->result_type.type != YZ_PTR)
		return err_print_pos(__func__, NULL,
				parser->lexer.line, parser->lexer.column);
	if (identifier_assign_get_val(parser, &ptr->ref, &val))
		return 1;
	if (backend_call(ptr_set_val)(ident, val, mode))
		return err_print_pos(__func__, "Backend call failed!",
				parser->lexer.line, parser->lexer.column);
	free_yz_val(val);
	return 0;
}

int ptr_set_val_handle_expr(struct expr *expr)
{
	free_yz_val(expr->vall);
	expr->vall = NULL;
	return 0;
}
