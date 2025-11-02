/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "expr.h"
#include "let.h"
#include "utils.h"
#include "../compiler/let.h"
#include "../lexer.h"
#include "../parser.h"
#include "../type.h"
#include <assert.h>
#include <sclexer.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static void handle_mut(struct parser *parser, struct yz_ident *ident);

void handle_mut(struct parser *parser, struct yz_ident *ident)
{
	assert(parser && ident);
	assert(CUR_TOK(parser).kind == SCLEXER_IDENT);
	eat_tok(parser);
	ident->mut = true;
}

int parse_let(struct parser *parser)
{
	struct yz_ident *ident = calloc(1, sizeof(*ident));
	struct yz_expr *value;
	if (CUR_TOK(parser).kind != SCLEXER_IDENT)
		goto err_unexpected_tok;
	if (strcmp(CUR_TOK(parser).data.str.begin, "mut") == 0)
		handle_mut(parser, ident);
	if (get_ident_with_type(parser, &ident->name, &ident->type))
		return 1;
	if (ident->type.type == YZ_VOID)
		goto err_void;
	if (!eat_tok_with_sym(LEXER_SYM_INFIX_ASSIGN, parser))
		goto err_uninit;
	if (!(value = parse_expr(parser)))
		return 1;
	compile_let(&parser->mcb, ident, value);
	return 0;
err_unexpected_tok:
	printf(ERR_FMT"miss identifier name, unexpected token: '%s'\n",
			ERR_FMT_ARG(parser),
			get_token_str(&CUR_TOK(parser)));
	return 1;
err_void:
	printf(ERR_FMT"identifier type cannot be 'void'\n",
			ERR_FMT_ARG(parser));
	return 1;
err_uninit:
	printf(ERR_FMT"identifier is uninitialized!\n",
			ERR_FMT_ARG(parser));
	return 1;
}
