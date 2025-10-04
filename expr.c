/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "expr.h"
#include "literal.h"
#include <stdio.h>

static void print_yz_expr_binary(const struct yz_expr *self, uint16_t depth);
static void print_yz_expr_term(const struct yz_literal *self, uint16_t depth);

void print_yz_expr_binary(const struct yz_expr *self, uint16_t depth)
{
	printf("%*s<type(binary): '%d'>\n", depth, "", self->type);
	print_yz_expr_term(self->data.binary.lhs, depth);
	print_yz_expr_term(self->data.binary.rhs, depth);
}

void print_yz_expr_term(const struct yz_literal *self, uint16_t depth)
{
	switch (self->type.type) {
	case YZ_EXPR:
		print_yz_expr(self->data.expr, depth + 1);
		break;
	case YZ_I8: case YZ_I16: case YZ_I32: case YZ_I64:
		printf("%*sterm: <type: '%s'> <value: '%ld'>\n",
				depth, "",
				type_get_str(self->type.type),
				self->data.sint);
		break;
	case YZ_U8: case YZ_U16: case YZ_U32: case YZ_U64:
		printf("%*sterm: <type: '%s'> <value: '%lu'>\n",
				depth, "",
				type_get_str(self->type.type),
				self->data.uint);
		break;
	default:
		printf("%*sterm: <type: '%s'>\n",
				depth, "",
				type_get_str(self->type.type));
		break;
	}
}

void free_yz_expr(struct yz_expr *self)
{
	if (!self)
		return;
	switch (self->type) {
	case YZ_EXPR_TERM_LITERAL:
		free_yz_literal(self->data.term);
		return;
	default: break;
	}
	if (self->type >= YZ_EXPR_BINARY_ADD
			&& self->type <= YZ_EXPR_BINARY_SUB_ASSIGN) {
		free_yz_literal(self->data.binary.lhs);
		free_yz_literal(self->data.binary.rhs);
	}
}

enum YZ_EXPR_TYPE map_tok_type2expr_type(enum TOK_TYPE tok)
{
	switch (tok) {
	case TOK_INFIX_ASSIGN:     return YZ_EXPR_BINARY_ASSIGN;
	case TOK_INFIX_ADD:        return YZ_EXPR_BINARY_ADD;
	case TOK_INFIX_ADD_ASSIGN: return YZ_EXPR_BINARY_ADD_ASSIGN;
	case TOK_INFIX_DIV:        return YZ_EXPR_BINARY_DIV;
	case TOK_INFIX_DIV_ASSIGN: return YZ_EXPR_BINARY_DIV_ASSIGN;
	case TOK_INFIX_MUL:        return YZ_EXPR_BINARY_MUL;
	case TOK_INFIX_MUL_ASSIGN: return YZ_EXPR_BINARY_MUL_ASSIGN;
	case TOK_INFIX_SUB:        return YZ_EXPR_BINARY_SUB;
	case TOK_INFIX_SUB_ASSIGN: return YZ_EXPR_BINARY_SUB_ASSIGN;
	default: break;
	}
	return -1;
}

void print_yz_expr(const struct yz_expr *self, uint16_t depth)
{
	if (!self)
		return;
	printf("%*s\x1b[33mstruct yz_expr {\x1b[0m\n", depth, "");
	switch (self->type) {
	case YZ_EXPR_TERM_LITERAL:
		print_yz_expr_term(self->data.term, depth);
		break;
	case YZ_EXPR_BINARY_ADD:
	case YZ_EXPR_BINARY_ADD_ASSIGN:
	case YZ_EXPR_BINARY_DIV:
	case YZ_EXPR_BINARY_DIV_ASSIGN:
	case YZ_EXPR_BINARY_MUL:
	case YZ_EXPR_BINARY_MUL_ASSIGN:
	case YZ_EXPR_BINARY_SUB:
	case YZ_EXPR_BINARY_SUB_ASSIGN:
		print_yz_expr_binary(self, depth);
		break;
	default:
		break;
	}
	printf("%*s\x1b[33m}\x1b[0m\n", depth, "");
}
