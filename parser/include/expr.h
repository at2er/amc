/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef AMC_PARSER_EXPR_H
#define AMC_PARSER_EXPR_H
#include "../../include/expr.h"
#include "../../include/symbol.h"

int expr_apply(struct parser *parser, struct expr *e);
struct expr *parse_expr(struct parser *parser, int top);
struct expr *parse_expr_unary(struct parser *parser, char index);

#endif
