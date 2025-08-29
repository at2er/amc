/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "include/array.h"
#include "include/expr.h"
#include "include/identifier.h"
#include "include/op.h"
#include "include/ptr.h"
#include "include/struct.h"
#include "../include/backend.h"
#include "../include/checker/ptr.h"
#include "../include/parser.h"
#include "../include/ptr.h"
#include "../utils/utils.h"
#include <stdlib.h>

static int op_unary_extract_val(struct parser *parser, struct expr *e);
static int op_unary_get_addr(struct parser *parser, struct expr *e);

static int (*op_special_f[])(struct parser *parser, struct expr *e) = {
	op_unary_extract_val,
	op_unary_get_addr
};

static int op_extract_val_check(struct parser *parser, yz_val *v);
static int op_extract_val_handle_expr(struct expr *e);
static int op_extract_val_handle_expr_from_sym(struct expr *e,
		struct symbol *sym);
static int op_get_addr_check(yz_val *v);
static int op_get_addr_handle_val(struct expr *e, int from_extracted_val);
static struct symbol *op_get_ptr(struct parser *parser, yz_val *v);
static struct symbol *op_get_ptr_from_expr(struct parser *parser, struct expr *e);

static int op_assign_extracted_val(struct parser *parser, struct expr *e);
static int op_assign_get_vall_expr(struct expr *e, struct symbol **result);
static int op_assign_get_vall_sym(struct symbol *sym, struct symbol **result);

int op_unary_extract_val(struct parser *parser, struct expr *e)
{
	if (!EXPR_IS_UNARY(e))
		return 1;
	if (e->valr->type.type == AMC_SYM) {
		if (op_extract_val_check(parser, e->valr))
			goto err_check_failed;
		if (op_extract_val_handle_expr(e))
			return 1;
	} else if (e->valr->type.type != AMC_EXTRACT_VAL) {
		return 1;
	}
	if (backend_call(ops[e->op])(e))
		goto err_backend_failed;
	return 0;
err_check_failed:
	backend_stop(BE_STOP_SIGNAL_ERR);
	return 1;
err_backend_failed:
	printf("amc: op_unary_extract_val: Backend failed!\n");
	backend_stop(BE_STOP_SIGNAL_ERR);
	return 1;
}

int op_unary_get_addr(struct parser *parser, struct expr *e)
{
	int ret = 0;
	if (!EXPR_IS_UNARY(e))
		return 1;
	if ((ret = op_get_addr_check(e->valr)) > 0)
		goto err_check_failed;
	if (ret == -1)
		if (expr_apply(parser, e->valr->data.v))
			return 1;
	if (op_get_addr_handle_val(e, ret == -1))
		goto err_check_failed;
	if (backend_call(ops[e->op])(e))
		goto err_backend_failed;
	return 0;
err_check_failed:
	backend_stop(BE_STOP_SIGNAL_ERR);
	return 1;
err_backend_failed:
	printf("amc: op_unary_get_addr: Backend failed!\n");
	backend_stop(BE_STOP_SIGNAL_ERR);
	return 1;
}

int op_extract_val_check(struct parser *parser, yz_val *v)
{
	struct symbol *ptr = NULL;
	if ((ptr = op_get_ptr(parser, v)) == NULL)
		return 1;
	if (ptr->result_type.type != YZ_PTR)
		goto err_not_ptr;
	if (!check_ptr_can_use(ptr))
		return 1;
	return 0;
err_not_ptr:
	printf("amc: op_extract_val_check: Value is not pointer!\n");
	backend_stop(BE_STOP_SIGNAL_ERR);
	return 1;
}

int op_extract_val_handle_expr(struct expr *e)
{
	switch (e->valr->type.type) {
	case AMC_SYM:
		return op_extract_val_handle_expr_from_sym(e, e->valr->data.v);
		break;
	case AMC_EXPR:
		if (op_extract_val_handle_expr(e->valr->data.v))
			return 1;
		e->sum_type = ((struct expr*)e->valr->data.v)->sum_type;
		return 0;
		break;
	case AMC_EXTRACT_VAL:
		return op_extract_val_handle_expr_from_sym(e,
				yz_get_extracted_val(e->valr->data.v));
		break;
	default: break;
	}
	printf("amc: op_extract_val_handle_expr: Unsupport type!\n");
	return 1;
}

int op_extract_val_handle_expr_from_sym(struct expr *e, struct symbol *sym)
{
	if (sym == NULL)
		goto err_sym_null;
	if (sym->result_type.type != YZ_PTR)
		goto err_unsupport_type;
	e->sum_type = &((yz_ptr_type*)sym->result_type.v)->ref;
	return 0;
err_sym_null:
	printf("amc: op_extract_val_handle_expr_from_sym: Symbol null!\n");
	return 1;
err_unsupport_type:
	printf("amc: op_extract_val_handle_expr_from_sym: Unsupport type!\n");
	return 1;
}

int op_get_addr_check(yz_val *v)
{
	struct expr *expr = NULL;
	struct symbol *sym = NULL;
	if (v->type.type == AMC_SYM) {
		sym = v->data.v;
		if (sym->type != SYM_IDENTIFIER)
			goto err_val_not_identifier;
		return 0;
	}
	if (v->type.type != AMC_EXPR)
		return 1;
	expr = v->data.v;
	if (expr->op != OP_EXTRACT_VAL
			|| expr->valr->type.type != AMC_EXTRACT_VAL)
		goto err_not_extracted_val;
	return -1;
err_val_not_identifier:
	printf("amc: op_get_addr_check: Value is not an identifier!\n");
	return 1;
err_not_extracted_val:
	printf("amc: op_get_addr_check: Value is not an extracted value!\n");
	return 1;
}

int op_get_addr_handle_val(struct expr *e, int from_extracted_val)
{
	yz_ptr *ptr = malloc(sizeof(*ptr));
	yz_ptr_type *ptr_type = malloc(sizeof(*ptr_type));
	if (!from_extracted_val) {
		ptr->ref.data.v = e->valr->data.v;
		ptr->ref.type.type = AMC_SYM;
		ptr->ref.type.v = ptr->ref.data.v;
		ptr_type->flag_mut = ((struct symbol*)ptr->ref.data.v)
			->flags.mut;
	} else {
		ptr->ref.data.v = e->valr->data.expr->valr->data.v;
		ptr->ref.type.type = AMC_EXTRACT_VAL;
		ptr->ref.type.v = ptr->ref.data.v;
		ptr_type->flag_mut = ((yz_extract_val*)ptr->ref.data.v)
			->elem->flags.mut;
		e->valr->data.expr->valr->data.v = NULL;
		free_expr(e->valr->data.expr);
	}
	ptr_type->ref.type = ptr->ref.type.type;
	ptr_type->ref.v = ptr->ref.type.v;
	ptr_type->level = 1;
	e->valr->data.v = ptr;
	e->valr->type.type = YZ_PTR;
	e->valr->type.v = ptr_type;
	e->sum_type = &e->valr->type;
	return 0;
}

struct symbol *op_get_ptr(struct parser *parser, yz_val *v)
{
	struct symbol *ptr = NULL;
	if (v->type.type == AMC_EXPR)
		return op_get_ptr_from_expr(parser, v->data.v);
	if (v->type.type != AMC_SYM)
		goto err_val_not_ptr;
	ptr = v->data.v;
	if (ptr->type != SYM_IDENTIFIER && ptr->type != SYM_FUNC_ARG)
		goto err_val_not_ptr;
	return ptr;
err_val_not_ptr:
	printf("amc: op_get_ptr: Value is not pointer!\n");
	return NULL;
}

struct symbol *op_get_ptr_from_expr(struct parser *parser, struct expr *e)
{
	struct symbol *ptr = NULL;
	yz_extract_val *src = NULL;
	if (e->op != OP_EXTRACT_VAL)
		goto err_not_extracted_val;
	if (e->valr->type.type != AMC_EXTRACT_VAL)
		goto err_not_extracted_val;
	src = e->valr->data.v;
	ptr = src->elem;
	if (expr_apply(parser, e) > 0)
		goto err_cannot_apply_expr;
	return ptr;
err_not_extracted_val:
	printf("amc: op_get_ptr_from_expr: Value is not extracted value!\n");
	return NULL;
err_cannot_apply_expr:
	printf("amc: op_get_ptr_from_expr: Cannot apply expression!\n");
	return NULL;
}

int op_assign_extracted_val(struct parser *parser, struct expr *e)
{
	yz_extract_val *src = NULL;
	yz_val *val = NULL;
	if (e->vall->type.type != AMC_EXPR)
		return 1;
	val = e->vall->data.expr->valr;
	if (val->type.type == AMC_SYM) {
		if (ptr_set_val(parser, val->data.v, e->op))
			return 1;
		return ptr_set_val_handle_expr(e);
	}
	if (val->type.type != AMC_EXTRACT_VAL)
		return 1;
	src = val->data.v;
	switch (src->type) {
	case YZ_EXTRACT_ARRAY:
		if (array_set_elem(parser, src->sym, src->data.offset, e->op))
			return 1;
		break;
	case YZ_EXTRACT_STRUCT:
		if (struct_set_elem(parser, src->sym, src->data.index, e->op))
			return 1;
		break;
	case YZ_EXTRACT_STRUCT_FROM_PTR:
		if (struct_set_elem_from_ptr(parser, src->sym, src->data.index,
					e->op))
			return 1;
		break;
	default:
		break;
	}
	free_expr(e->vall->data.expr);
	e->vall->type.type = AMC_ERR_TYPE;
	e->vall->type.v = NULL;
	e->vall->data.v = NULL;
	return 0;
}

int op_assign_get_vall_expr(struct expr *e, struct symbol **result)
{
	if (e->op != OP_EXTRACT_VAL)
		return 1;
	if (e->valr->type.type == AMC_SYM) {
		if (e->valr->data.sym->result_type.type != YZ_PTR)
			return 1;
		*result = e->valr->data.sym;
		return 0;
	}
	if (e->valr->type.type != AMC_EXTRACT_VAL)
		return 1;
	return 0;
}

int op_assign_get_vall_sym(struct symbol *sym, struct symbol **result)
{
	if (sym->type != SYM_IDENTIFIER)
		return 1;
	*result = sym;
	return 0;
}

int op_apply_special(struct parser *parser, struct expr *e)
{
	int func_id = 0;
	if (REGION_INT(e->op, OP_ASSIGN, OP_ASSIGN_SUB))
		return 0;
	func_id = e->op - OP_SPECIAL_START - 5;
	if (func_id < LENGTH(op_special_f))
		return op_special_f[func_id](parser, e);
	return 0;
}

int op_assign(struct parser *parser, struct expr *e)
{
	struct symbol *sym = NULL;
	if (e->vall == NULL && e->valr == NULL)
		return 0;
	if (e->vall->type.type == AMC_SYM) {
		if (op_assign_get_vall_sym(e->vall->data.sym, &sym))
			return 1;
		return identifier_assign_val(parser, sym, e->op);
	}
	if (e->vall->type.type != AMC_EXPR)
		return 1;
	if (op_assign_get_vall_expr(e->vall->data.expr, &sym))
		return 1;
	return op_assign_extracted_val(parser, e);
}

struct expr *op_extract_val_expr_create(yz_type *sum_type,
		yz_extract_val *val)
{
	struct expr *expr = malloc(sizeof(*expr));
	expr->vall = NULL;
	expr->valr = malloc(sizeof(*expr->valr));
	expr->valr->data.v = val;
	expr->valr->type.type = AMC_EXTRACT_VAL;
	expr->valr->type.v = expr->valr->data.v;
	expr->op = OP_EXTRACT_VAL;
	expr->op_power = 0;
	expr->sum_type = sum_type;
	return expr;
}
