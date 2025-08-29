/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "include/block.h"
#include "include/expr.h"
#include "include/identifier.h"
#include "include/indent.h"
#include "include/lexer.h"
#include "include/utils.h"
#include "../include/backend.h"
#include "../include/checker/ptr.h"
#include "../include/checker/symbol.h"
#include "../include/checker/type.h"
#include "../include/expr.h"
#include "../include/parser.h"
#include "../include/ptr.h"
#include "../include/symbol.h"
#include "../utils/die.h"
#include "../utils/str/str.h"
#include <sclexer.h>
#include <stdio.h>
#include <string.h>

static int identifier_assign_backend_call(struct symbol *sym, yz_val *val,
		enum OP_ID mode);
static enum TRY_RESULT identifier_try_handle_null(struct parser *parser,
		struct symbol *ident, yz_val *val);

int identifier_assign_backend_call(struct symbol *sym, yz_val *val,
		enum OP_ID mode)
{
	if (sym->flags.mut) {
		if (backend_call(var_set)(sym, val, mode))
			return 1;
	} else {
		if (mode != OP_ASSIGN)
			goto err_unsupport_op;
		if (backend_call(var_immut_init)(sym, val))
			return 1;
	}
	sym->flags.is_init = 1;
	return 0;
err_unsupport_op:
	printf("amc: identifier_assign_backend_call: "
			"Unsupport operator for immutable identifier!\n");
	return 1;
}

enum TRY_RESULT
identifier_try_handle_null(struct parser *parser, struct symbol *ident,
		yz_val *val)
{
	backend_null_handle *handle;
	struct sclexer lexer_record;
	long orig_pos = sclexer_record(&parser->lexer, &lexer_record);
	if (orig_pos == -1)
		return TRY_RESULT_FAULT;
	if (sclexer_get_line(&parser->lexer))
		return TRY_RESULT_FAULT;
	if (indent_read(&parser->lexer) != parser->scope->indent)
		goto not_handled_restore_pos;
	if (lexer_try_read_str(&parser->lexer, "|?", 2)
			!= TRY_RESULT_HANDLED)
		goto err_maybe_null;
	((yz_ptr_type*)ident->result_type.v)->flag_checked_null = 1;
	if (backend_call(null_handle_begin)(&handle, val))
		return TRY_RESULT_FAULT;
	if (parse_block(parser))
		return TRY_RESULT_FAULT;
	if (backend_call(null_handle_end)(handle))
		return TRY_RESULT_FAULT;
	return TRY_RESULT_HANDLED;
not_handled_restore_pos:
	if (sclexer_restore(&parser->lexer, &lexer_record, orig_pos))
		return TRY_RESULT_FAULT;
	return TRY_RESULT_NOT_HANDLED;
err_maybe_null:
	printf(LEXER_ERR_FMT ERROR_STR":\n"
			"| Assign a can null value to a cannot be null "
			"identifier: '%s'.\n"
			"| "HINT_STR": "
			"Append '|? =>' to next line to handle null branch\n",
			LEXER_ERR_FMT_ARG(parser->lexer),
			ident->name.s);
	return TRY_RESULT_FAULT;
}

int identifier_assign_get_val(struct parser *parser,
		yz_type *dest_type, yz_val **result)
{
	struct expr *expr = NULL;
	uint64_t orig_column = parser->lexer.column,
	         orig_line = parser->lexer.line;
	if ((expr = parse_expr(parser, 1)) == NULL)
		return err_print_pos(__func__, "Cannot parse expr!",
				orig_line, orig_column);
	if (expr_apply(parser, expr) > 0)
		goto err_cannot_apply_expr;
	if ((*result = identifier_handle_expr_val(expr, dest_type)) == NULL)
		return err_print_pos(__func__, "Cannot handle expr value!",
				orig_line, orig_column);
	return 0;
err_cannot_apply_expr:
	free_expr(expr);
	return err_print_pos(__func__, "Cannot apply expr!",
			orig_line, orig_column);
}

int identifier_assign_val(struct parser *parser, struct symbol *sym,
		enum OP_ID mode)
{
	uint64_t orig_column = parser->lexer.column,
	         orig_line = parser->lexer.line;
	int ret = 0;
	yz_val *val = NULL;
	if (sym->result_type.type == YZ_PTR)
		((yz_ptr_type*)sym->result_type.v)->flag_checked_null = 0;
	if (!check_sym_can_assign(sym))
		return err_print_pos(__func__, NULL, orig_line, orig_column);
	if (identifier_assign_get_val(parser, &sym->result_type, &val))
		return 1;
	if (!(ret = identifier_check_can_assign_val(parser, sym, val)))
		goto err_free_val;
	if (identifier_assign_backend_call(sym, val, mode))
		goto err_print_pos;
	free_yz_val(val);
	return 0;
err_print_pos:
	err_print_pos(__func__, "Backend call failed!",
			orig_line, orig_column);
err_free_val:
	free_yz_val(val);
	return 1;
}

int identifier_check_can_assign_val(struct parser *parser,
		struct symbol *ident, yz_val *val)
{
	yz_ptr_type *ptr = NULL,
	            *ident_ptr = ident->result_type.v;
	enum TRY_RESULT ret = 0;
	if (ident->result_type.type != YZ_PTR || ident_ptr->flag_can_null)
		return 1;
	if (val->type.type == YZ_NULL)
		return 0;
	if (val->type.type == AMC_EXPR && val->data.expr->op == OP_GET_ADDR) {
		if (check_ptr_get_addr_to_ident(val->data.expr, ident))
			goto err_print_pos;
		return 1;
	}
	if (val->type.type == AMC_SYM && val->data.sym->type == SYM_FUNC) {
		ptr = val->data.sym->result_type.v;
		if (!ptr->flag_can_null)
			return 1;
		ret = identifier_try_handle_null(parser, ident, val);
		if (ret != TRY_RESULT_HANDLED)
			return 0;
		return 1;
	}
	return 1;
err_print_pos:
	err_print_pos(__func__, NULL,
			parser->lexer.line,
			parser->lexer.column);
	return 0;
}

int identifier_check_mut(struct sclexer *lexer)
{
	int ret = lexer_try_read_str(lexer, "mut", 3);
	if (ret == TRY_RESULT_FAULT)
		die("Failed to read token\n");
	return ret;
}

yz_val *identifier_handle_expr_val(struct expr *e, yz_type *type)
{
	yz_val *val = expr2yz_val(e);
	if (val == NULL)
		goto err_free_val;
	if (identifier_handle_val_type(&val->type, type))
		goto err_free_val;
	return val;
err_free_val:
	free_yz_val(val);
	return NULL;
}

int identifier_handle_val_type(yz_type *src, yz_type *dest)
{
	if (!check_type_equal(src, dest))
		return 1;
	if (!YZ_IS_DIGIT(src->type) || !YZ_IS_DIGIT(dest->type))
		return 0;
	src->type = dest->type;
	src->v = dest->v;
	return 0;
}
