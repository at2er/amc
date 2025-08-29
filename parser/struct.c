/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "include/constructor.h"
#include "include/expr.h"
#include "include/identifier.h"
#include "include/indent.h"
#include "include/keywords.h"
#include "include/struct.h"
#include "include/lexer.h"
#include "include/op.h"
#include "include/type.h"
#include "include/utils.h"
#include "../include/backend.h"
#include "../include/checker/struct.h"
#include "../include/checker/symbol.h"
#include "../include/parser.h"
#include "../include/ptr.h"
#include <sclexer.h>
#include <sctrie.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int constructor_struct_elem(struct constructor_handle *handle);
static int struct_def_read_elem(struct parser *parser, yz_struct *src);
static int struct_def_read_elems(struct parser *parser, yz_struct *src);
static int struct_def_read_name(struct sclexer *lexer, str *name);
static int struct_def_reg(yz_struct *src, struct scope *scope);
static int struct_def_reg_elem(yz_struct *src, struct symbol *elem);
static int struct_get_elem_handle_val(yz_val *val, int index,
		struct symbol *elem, enum YZ_EXTRACT_VAL_TYPE type);
/**
 * @return: elem index.
 */
static int struct_get_elem_read_elem(str *name, yz_struct *src);

int constructor_struct_elem(struct constructor_handle *handle)
{
	struct expr *expr = NULL;
	yz_struct *src = handle->sym->result_type.v;
	struct lexer_tok tok;
	yz_val *val = NULL;
	if (handle->index > src->elem_count - 1)
		goto err_too_many_elem;
	if ((expr = parse_expr(handle->parser, 1)) == NULL)
		goto err_cannot_parse_expr;
	if (expr_apply(handle->parser, expr) > 0)
		goto err_cannot_apply_expr;
	if ((val = identifier_handle_expr_val(expr,
					&src->elems[handle->index]
					->result_type)) == NULL)
		goto err_cannot_apply_expr;
	src->elems[handle->index]->flags.is_init = 1;
	handle->vs[handle->index] = val;
	handle->index += 1;
	if (lexer_read_tok(&tok, &handle->parser->lexer))
		return LEXER_RESULT_FAULT;
	switch (tok.type) {
	case TOK_TYPE_COMMA:
		return LEXER_RESULT_CONTINUE;
	case TOK_TYPE_PAREN_R:
		return LEXER_RESULT_END;
	default: break;
	}
	return LEXER_RESULT_FAULT;
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

int struct_def_read_elem(struct parser *parser, yz_struct *src)
{
	struct symbol *elem = NULL;
	struct sclexer lexer_record;
	uint64_t orig_pos = sclexer_record(&parser->lexer, &lexer_record);
	if (orig_pos == -1)
		return LEXER_RESULT_FAULT;
	if (sclexer_get_line(&parser->lexer) == EOF)
		return LEXER_RESULT_FAULT;
	if (indent_read(&parser->lexer) != parser->scope->indent) {
		if (sclexer_restore(&parser->lexer, &lexer_record, orig_pos))
			return LEXER_RESULT_FAULT;
		return LEXER_RESULT_END;
	}
	elem = calloc(1, sizeof(*elem));
	elem->type = SYM_STRUCT_ELEM;
	elem->flags.mut = identifier_check_mut(&parser->lexer);
	if (parse_type_name_pair(parser, &elem->name, &elem->result_type))
		goto err_free_elem;
	if (struct_def_reg_elem(src, elem))
		goto err_free_elem;
	return LEXER_RESULT_CONTINUE;
err_free_elem:
	free(elem);
	return LEXER_RESULT_FAULT;
}

int struct_def_read_elems(struct parser *parser, yz_struct *src)
{
	int orig_indent = parser->scope->indent,
	    ret = 0;
	parser->scope->indent += 1;
	while ((ret = struct_def_read_elem(parser, src))
			!= LEXER_RESULT_END) {
		if (ret == LEXER_RESULT_FAULT)
			return 1;
	}
	parser->scope->indent = orig_indent;
	return 0;
}

int struct_def_read_name(struct sclexer *lexer, str *name)
{
	struct lexer_tok tok;
	if (lexer_read_tok(&tok, lexer) || tok.type != TOK_TYPE_STR)
		return 1;
	if (name->len == 3 && strncmp("rec", name->s, name->len) == 0) {
		name->len = 0;
		if (lexer_read_tok(&tok, lexer) || tok.type != TOK_TYPE_STR)
			return 1;
		if (str_copy(&tok.data.s, name))
			return 1;;
		return -1;
	}
	return str_copy(&tok.data.s, name);
}

int struct_def_reg(yz_struct *self, struct scope *scope)
{
	yz_user_type *type = sctrie_append_elem(&scope->types, sizeof(*type),
			self->name.s, self->name.len);
	if (type == NULL)
		goto err_defined;
	type->type = YZ_STRUCT;
	type->data.struct_ = self;
	return 0;
err_defined:
	printf("amc: struct_def_reg: "
			"Type defined: '%s'\n", self->name.s);
	backend_stop(BE_STOP_SIGNAL_ERR);
	return 1;
}

int struct_def_reg_elem(yz_struct *src, struct symbol *elem)
{
	src->elem_count += 1;
	src->elems = realloc(src->elems, sizeof(*src->elems)
			* src->elem_count);
	src->elems[src->elem_count - 1] = elem;
	return 0;
}

int struct_get_elem_handle_val(yz_val *val, int index, struct symbol *elem,
		enum YZ_EXTRACT_VAL_TYPE type)
{
	yz_extract_val *v = malloc(sizeof(*v));
	v->data.index = index;
	v->sym = val->data.v;
	v->elem = elem;
	v->type = type;
	val->data.v = op_extract_val_expr_create(&elem->result_type, v);
	if (val->data.v == NULL)
		return 1;
	val->type.type = AMC_EXPR;
	val->type.v = val->data.v;
	return 0;
}

int struct_get_elem_read_elem(str *name, yz_struct *src)
{
	for (int i = 0; i < src->elem_count; i++) {
		if (name->len != src->elems[i]->name.len)
			continue;
		if (strncmp(name->s, src->elems[i]->name.s, name->len) == 0)
			return i;
	}
	return -1;
}

int constructor_struct(struct parser *parser, struct symbol *sym)
{
	struct constructor_handle handle = {
		.index = 0,
		.len = ((yz_struct*)sym->result_type.v)->elem_count,
		.parser = parser,
		.sym = sym,
		.vs = calloc(handle.len, sizeof(yz_val*))
	};
	int ret = 0;
	while ((ret = constructor_struct_elem(&handle)) != LEXER_RESULT_END) {
		if (ret == LEXER_RESULT_FAULT)
			goto err_free_handle;
	}
	if (backend_call(struct_def)(&sym->backend_status,
				handle.vs, handle.len))
		goto err_backend_failed;
	free_constructor_handle_no_self(&handle);
	return 0;
err_backend_failed:
	free_constructor_handle_no_self(&handle);
	printf(LEXER_ERR_FMT"Backend call failed!\n",
			LEXER_ERR_FMT_ARG(parser->lexer));
err_free_handle:
	free_constructor_handle_no_self(&handle);
	backend_stop(BE_STOP_SIGNAL_ERR);
	return 1;
}

int parse_struct(struct parser *parser)
{
	yz_struct *result = NULL;
	int ret = 0;
	struct lexer_tok tok;
	if ((ret = struct_def_read_name(&parser->lexer, &result->name)) > 0)
		return 1;
	result = calloc(1, sizeof(*result));
	if (ret == -1)
		result->flags.rec = 1;
	if (lexer_read_tok(&tok, &parser->lexer))
		goto err_free_result;
	if (tok.type != TOK_TYPE_BLOCK_START)
		goto err_free_result;
	if (struct_def_read_elems(parser, result))
		goto err_free_result;
	if (struct_def_reg(result, parser->stat.has_pub
				? parser->scope_pub
				: parser->scope))
		goto err_free_result;
	return 0;
err_free_result:
	str_free_noself(&result->name);
	free(result);
	return 1;
}

int struct_get_elem(struct parser *parser, yz_val *val)
{
	int ret = 0;
	struct symbol *sym = val->data.v;
	yz_struct *src;
	struct lexer_tok tok;
	enum YZ_EXTRACT_VAL_TYPE type = YZ_EXTRACT_STRUCT;
	src = sym->result_type.v;
	if (sym->result_type.type != YZ_STRUCT)
		goto err_not_struct;
	if (sym->result_type.type == YZ_PTR) {
		if (((yz_ptr_type*)sym->result_type.v)->ref.type != YZ_STRUCT)
			goto err_not_struct;
		src = ((yz_ptr_type*)sym->result_type.v)->ref.v;
		type = YZ_EXTRACT_STRUCT_FROM_PTR;
	}
	if (lexer_read_tok(&tok, &parser->lexer) || tok.type != TOK_TYPE_STR)
		return 1;
	if ((ret = struct_get_elem_read_elem(&tok.data.s, src)) == -1)
		goto err_print_pos;
	return struct_get_elem_handle_val(val, ret, src->elems[ret], type);
err_not_struct:
	printf(LEXER_ERR_FMT"Symbol: '%s' not a struct!\n",
			LEXER_ERR_FMT_ARG(parser->lexer),
			sym->name.s);
	backend_stop(BE_STOP_SIGNAL_ERR);
	return 1;
err_print_pos:
	return err_print_pos(__func__, NULL, parser->lexer.line,
			parser->lexer.column);
}

int struct_set_elem(struct parser *parser, struct symbol *sym, int index,
		enum OP_ID mode)
{
	uint64_t orig_column = parser->lexer.column,
	         orig_line = parser->lexer.line;
	yz_val *val = NULL;
	struct symbol *elem = ((yz_struct*)sym->result_type.v)->elems[index];
	if (elem->result_type.type == YZ_PTR)
		((yz_ptr_type*)elem->result_type.v)->flag_checked_null = 0;
	if (!check_struct_elem_can_assign(sym, elem))
		return err_print_pos(__func__, NULL, orig_line, orig_column);
	if (identifier_assign_get_val(parser, &elem->result_type, &val))
		return 1;
	if (!check_sym_can_assign_val(elem, val))
		goto err_cannot_assign;
	if (backend_call(struct_set_elem)(sym, index, val, mode))
		goto err_backend_failed;
	free_yz_val(val);
	return 0;
err_cannot_assign:
	free_yz_val(val);
	return err_print_pos(__func__, NULL, orig_line, orig_column);
err_backend_failed:
	free_yz_val(val);
	return err_print_pos(__func__, "Backend call failed!",
			orig_line, orig_column);
}

int struct_set_elem_from_ptr(struct parser *parser, struct symbol *sym,
		int index, enum OP_ID mode)
{
	struct symbol *elem = NULL;
	u64 orig_column = parser->lexer.column,
	    orig_line = parser->lexer.line;
	yz_ptr_type *struct_ref = sym->result_type.v;
	yz_val *val = NULL;
	if (sym->result_type.type != YZ_PTR)
		return 1;
	if (struct_ref->ref.type != YZ_STRUCT)
		return 1;
	elem = ((yz_struct*)struct_ref->ref.v)->elems[index];
	if (elem->result_type.type == YZ_PTR)
		((yz_ptr_type*)elem->result_type.v)->flag_checked_null = 0;
	if (!check_struct_elem_can_assign(sym, elem))
		return err_print_pos(__func__, NULL, orig_line, orig_column);
	if (identifier_assign_get_val(parser, &elem->result_type, &val))
		return 1;
	if (!check_sym_can_assign_val(elem, val))
		return err_print_pos(__func__, NULL, orig_line, orig_column);
	if (backend_call(struct_set_elem_from_ptr)(sym, index, val, mode))
		return err_print_pos(__func__, "Backend call failed!",
				orig_line, orig_column);
	free_yz_val(val);
	return 0;
}
