/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef AMC_EXPR_H
#define AMC_EXPR_H
#include "token.h"
#include <stdint.h>

struct yz_literal;

enum YZ_EXPR_TYPE {
	YZ_EXPR_TERM_LITERAL,

	YZ_EXPR_BINARY_ASSIGN,
	YZ_EXPR_BINARY_ADD,
	YZ_EXPR_BINARY_ADD_ASSIGN,
	YZ_EXPR_BINARY_DIV,
	YZ_EXPR_BINARY_DIV_ASSIGN,
	YZ_EXPR_BINARY_MUL,
	YZ_EXPR_BINARY_MUL_ASSIGN,
	YZ_EXPR_BINARY_SUB,
	YZ_EXPR_BINARY_SUB_ASSIGN,
	YZ_EXPR_TYPE_COUNT
};

union yz_expr_data {
	struct yz_literal *term;
	struct {
		struct yz_literal *lhs, *rhs;
	} binary;
};

struct yz_expr {
	union yz_expr_data data;
	enum YZ_EXPR_TYPE type;

	struct yz_type *sum_type;
};

void free_yz_expr(struct yz_expr *self);

/**
 * @return: `enum YZ_EXPR_TYPE` on success, otherwise -1
 */
enum YZ_EXPR_TYPE map_tok_type2expr_type(enum TOK_TYPE tok);

void print_yz_expr(const struct yz_expr *self, uint16_t depth);

#endif
