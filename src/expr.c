/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "expr.h"
#include "lexer.h"
#include "literal.h"
#include "type.h"
#include "utils.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

static void merge_integer_literal_type(enum YZ_TYPE sum, struct yz_literal *src);
static void print_yz_expr_binary(const struct yz_expr *self, uint16_t depth);
static enum YZ_TYPE type2literal_type(enum YZ_TYPE src);

void merge_integer_literal_type(enum YZ_TYPE sum, struct yz_literal *src)
{
	if (yz_type_is_integer_literal(src->type.type)) {
		src->type.type = sum;
		return;
	}
	switch (src->type.type) {
	case YZ_EXPR:
		merge_expr_integer_literal_type(sum, src->data.expr);
		return;
	default: break;
	}
}

void print_yz_expr_binary(const struct yz_expr *self, uint16_t depth)
{
	printf("%*s<type(binary): '%s'>\n",
			depth, "",
			get_yz_expr_type_str(self->type));
	print_yz_literal(self->data.binary.lhs, depth);
	print_yz_literal(self->data.binary.rhs, depth);
}

enum YZ_TYPE type2literal_type(enum YZ_TYPE src)
{
	if (src >= YZ_I8 && src <= YZ_U64)
		return src + YZ_I8_LITERAL;
	return src;
}

const char *get_yz_expr_type_str(enum YZ_EXPR_TYPE type)
{
	switch (type) {
	case YZ_EXPR_TERM_LITERAL:      return "term";
	case YZ_EXPR_BINARY_ADD:        return "+";
	case YZ_EXPR_BINARY_ADD_ASSIGN: return "+=";
	case YZ_EXPR_BINARY_DIV:        return "/";
	case YZ_EXPR_BINARY_DIV_ASSIGN: return "/=";
	case YZ_EXPR_BINARY_MUL:        return "*";
	case YZ_EXPR_BINARY_MUL_ASSIGN: return "*=";
	case YZ_EXPR_BINARY_SUB:        return "-";
	case YZ_EXPR_BINARY_SUB_ASSIGN: return "-=";
	default: break;
	}
	return NULL;
}

void merge_expr_integer_literal_type(enum YZ_TYPE sum, struct yz_expr *src)
{
	if (!yz_type_is_integer(sum))
		return;
	sum = type2literal_type(sum);
	if (src->type == YZ_EXPR_TERM_LITERAL) {
		merge_integer_literal_type(sum, src->data.term);
		return;
	}
	if (YZ_IS_BINARY_EXPR(src->type)) {
		merge_integer_literal_type(sum, src->data.binary.lhs);
		merge_integer_literal_type(sum, src->data.binary.rhs);
		return;
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
	if (YZ_IS_BINARY_EXPR(self->type)) {
		free_yz_literal(self->data.binary.lhs);
		free_yz_literal(self->data.binary.rhs);
	}
}

enum YZ_EXPR_TYPE map_tok_type2expr_type(enum LEXER_SYMBOLS tok)
{
	switch (tok) {
	case LEXER_SYM_INFIX_ASSIGN:     return YZ_EXPR_BINARY_ASSIGN;
	case LEXER_SYM_INFIX_ADD:        return YZ_EXPR_BINARY_ADD;
	case LEXER_SYM_INFIX_ADD_ASSIGN: return YZ_EXPR_BINARY_ADD_ASSIGN;
	case LEXER_SYM_INFIX_DIV:        return YZ_EXPR_BINARY_DIV;
	case LEXER_SYM_INFIX_DIV_ASSIGN: return YZ_EXPR_BINARY_DIV_ASSIGN;
	case LEXER_SYM_INFIX_MUL:        return YZ_EXPR_BINARY_MUL;
	case LEXER_SYM_INFIX_MUL_ASSIGN: return YZ_EXPR_BINARY_MUL_ASSIGN;
	case LEXER_SYM_INFIX_SUB:        return YZ_EXPR_BINARY_SUB;
	case LEXER_SYM_INFIX_SUB_ASSIGN: return YZ_EXPR_BINARY_SUB_ASSIGN;
	default: break;
	}
	return -1;
}

void print_yz_expr(const struct yz_expr *self, uint16_t depth)
{
	if (!self)
		return;
	printf("%*s\x1b[33mstruct yz_expr: type: '%s' {\x1b[0m\n", depth, "",
			type_get_str(self->sum_type->type));
	depth += PRINT_SHIFT;
	switch (self->type) {
	case YZ_EXPR_TERM_LITERAL:
		print_yz_literal(self->data.term, depth);
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
	depth -= PRINT_SHIFT;
	printf("%*s\x1b[33m}\x1b[0m\n", depth, "");
}
