/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "utils.h"
#include "../lexer.h"
#include "../type.h"
#include <assert.h>
#include <sclexer.h>

int get_ident_with_type(struct parser *parser,
		str *ident,
		struct yz_type *type)
{
	struct sclexer_tok *ident_tok, *colon, *type_tok;
	assert(parser && ident && type);
	if (parser->tokens_count - parser->cur_token < 3)
		return 1;
	ident_tok = peek_tok_with_kind(SCLEXER_IDENT,  parser, 0);
	colon     = peek_tok_with_sym(LEXER_SYM_COLON, parser, 1);
	type_tok  = peek_tok_with_kind(SCLEXER_IDENT,  parser, 2);
	if (!ident_tok || !colon || !type_tok)
		return 1;
	dup_tok_str2str(ident, ident_tok);
	type->type = type_get(TOK_IDENT_UNWRAP_REF(type_tok));
	if (type->type == -1)
		return 1;
	eat_tok(parser);
	eat_tok(parser);
	eat_tok(parser);
	return 0;
}
