/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef AMC_PARSER_EXPR_H
#define AMC_PARSER_EXPR_H
#include "parser.h"
#include "../expr.h"
#include <stdint.h>

struct yz_expr *parse_expr(struct parser *parser);

#endif
