/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "fn.h"
#include "pub.h"
#include "../keyword.h"
#include <sclexer.h>

int parse_pub(struct parser *parser)
{
	if (CUR_TOK(parser).kind != SCLEXER_KEYWORD)
		ereturnf(1, parser, "unexpected token '%s'",
				get_token_str(&CUR_TOK(parser)));
	switch (CUR_TOK(parser).data.keyword) {
	case KEYWORD_FN:
		eat_tok(parser);
		return parse_func_def(parser, YZ_SCOPE_PUBLIC);
	default:
		ereturnf(1, parser, "unexpected keyword '%s'",
				get_token_str(&CUR_TOK(parser)));
	}
	return 0;
}
