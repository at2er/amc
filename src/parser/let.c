/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "expr.h"
#include "let.h"
#include "utils.h"
#include "../compiler/let.h"
#include "../lexer.h"
#include "../object.h"
#include "../parser.h"
#include "../type.h"
#include <assert.h>
#include <sclexer.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static int get_sign(struct yz_ident *self, struct parser *parser);
static void handle_mut(struct yz_ident *self, struct parser *parser);

int get_sign(struct yz_ident *self, struct parser *parser)
{
	if (CUR_TOK(parser).kind != SCLEXER_IDENT)
		goto err_unexpected_tok;
	if (strcmp(CUR_TOK(parser).data.str.begin, "mut") == 0)
		handle_mut(self, parser);
	if (get_ident_with_type(parser, &self->name, &self->type))
		return 1;
	if (self->type.type == YZ_VOID)
		goto err_void;
	return 0;
err_unexpected_tok:
	eprintf(parser, "miss identifier name, unexpected token '%s'",
			get_token_str(&CUR_TOK(parser)));
	return 1;
err_void:
	eprint(parser, "identifier type cannot be 'void'");
	return 1;
}

void handle_mut(struct yz_ident *self, struct parser *parser)
{
	assert(parser && self);
	assert(CUR_TOK(parser).kind == SCLEXER_IDENT);
	eat_tok(parser);
	self->mut = true;
}

int parse_let(struct parser *parser)
{
	struct yz_ident *self = calloc(1, sizeof(*self));
	struct yz_symbol *wrapper;
	struct yz_expr *value;

	self->is_func_arg = false;
	if (get_sign(self, parser))
		goto err_free_self;

	if (!eat_tok_with_sym(LEXER_SYM_INFIX_ASSIGN, parser))
		goto err_uninit;
	if (!(value = parse_expr(parser)))
		goto err_free_self;

	self->object = create_yz_object(self->name.s);
	wrapper = append_symbol(&parser->symbols, &self->name);
	if (!wrapper)
		goto err_defined;
	wrapper->type = YZ_IDENT;
	wrapper->data.yz_ident = self;
	append_symbol_to_scope(wrapper, parser->cur_scope);

	compile_let(parser->mcb, self, value);
	return 0;
err_free_self:
	free_yz_ident(self);
	return 1;
err_uninit:
	eprint(parser, "identifier is uninitialized");
	goto err_free_self;
err_defined:
	eprint(parser, "symbol is defined");
	goto err_free_self;
}
