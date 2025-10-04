/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "parser/type.h"
#include "type.h"
#include <assert.h>

int parser_get_ident_with_type(struct parser *parser,
		str *ident,
		struct yz_type *type)
{
	struct lexer_tok *ident_tok, *colon, *type_tok;
	assert(parser);
	if ((ident_tok = CUR_TOK(parser)) == NULL)
		return 1;
	if (ident_tok->type != TOK_IDENT)
		return 1;
	if ((colon = CUR_TOK(parser)->next) == NULL)
		return 1;
	if (colon->type != TOK_COLON)
		return 1;
	if ((type_tok = CUR_TOK(parser)->next->next) == NULL)
		return 1;
	if (type_tok->type != TOK_IDENT)
		return 1;
	*ident = ident_tok->data.s;
	ident_tok->data.s.s = NULL;
	type->type = type_get(STR_UNWRAP(type_tok->data.s));
	if (type->type == -1)
		return 1;
	lexer_eat_tok(parser->lexer.cur_block);
	lexer_eat_tok(parser->lexer.cur_block);
	lexer_eat_tok(parser->lexer.cur_block);
	return 0;
}
