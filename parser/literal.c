/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "lexer.h"
#include "literal.h"
#include "parser.h"
#include "parser/expr.h"
#include "parser/literal.h"
#include "parser/utils.h"
#include <stdlib.h>

static struct yz_literal *parse_expr_literal(struct parser *parser);
static struct yz_literal *parse_number_literal(struct parser *parser);

struct yz_literal *parse_expr_literal(struct parser *parser)
{
	struct yz_literal *self = calloc(1, sizeof(*self));
	lexer_eat_tok(parser->lexer.cur_block);
	self->data.expr = parse_expr(parser);
	self->type.data.self = self->data.expr->sum_type;
	self->type.type = YZ_EXPR;
	if (lexer_eat_tok_with(TOK_PAREN_R, parser->lexer.cur_block))
		goto err_expr_not_end;
	return self;
err_expr_not_end:
	free_yz_literal(self);
	printf(ERR_FMT"expression not end\n", ERR_FMT_ARG(parser));
	return NULL;
}

struct yz_literal *parse_number_literal(struct parser *parser)
{
	struct yz_literal *self = calloc(1, sizeof(*self));
	self->type.type = YZ_I64;
	self->data.sint = CUR_TOK(parser)->data.sint;
	lexer_eat_tok(parser->lexer.cur_block);
	return self;
}

struct yz_literal *parse_literal(struct parser *parser)
{
	switch (CUR_TOK(parser)->type) {
	case TOK_INT:
	case TOK_INT_NEG:
		return parse_number_literal(parser);
	case TOK_PAREN_L:
		return parse_expr_literal(parser);
	default: break;
	}
	return NULL;
}
