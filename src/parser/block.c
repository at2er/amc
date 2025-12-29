/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "block.h"
#include "let.h"
#include "return.h"
#include "../die.h"
#include "../keyword.h"
#include "../panic.h"
#include "../parser.h"
#include <assert.h>
#include <sclexer.h>

enum PARSE_TOK_RESULT {
	PARSE_TOK_RESULT_CONTINUE,
	PARSE_TOK_RESULT_END,
	PARSE_TOK_RESULT_ERR
};

static int parse_keyword(struct parser *parser);
static enum PARSE_TOK_RESULT parse_tok(struct parser *parser);

int parse_keyword(struct parser *parser)
{
	assert(CUR_TOK(parser).kind == SCLEXER_KEYWORD);
	switch (CUR_TOK(parser).data.keyword) {
	case KEYWORD_LET:
		eat_tok(parser);
		return parse_let(parser);
	case KEYWORD_RETURN:
		eat_tok(parser);
		return parse_return(parser);
	default: break;
	}
	panicf("unsupport keyword: '%s'", get_token_str(&CUR_TOK(parser)));
	return 1;
}

enum PARSE_TOK_RESULT parse_tok(struct parser *parser)
{
	switch (CUR_TOK(parser).kind) {
	case SCLEXER_EOL:
		eat_tok(parser);
		break;
	case SCLEXER_KEYWORD:
		if (parse_keyword(parser))
			return PARSE_TOK_RESULT_ERR;
		return PARSE_TOK_RESULT_CONTINUE;
	case SCLEXER_INDENT_BLOCK_END:
		eat_tok(parser);
		return PARSE_TOK_RESULT_END;
	default: goto panic_unsupport_token; break;
	}
	return PARSE_TOK_RESULT_CONTINUE;
panic_unsupport_token:
	die(LOC_FMT PANIC_FMT"failed to parse token '%s'\n",
			LOC_FMT_ARG(parser), PANIC_FMT_ARG,
			get_token_str(&CUR_TOK(parser)));
	return PARSE_TOK_RESULT_ERR;
}

int parse_block(struct parser *parser)
{
	enum PARSE_TOK_RESULT ret;
	assert(CUR_TOK_IS_SYM(parser, LEXER_SYM_BLOCK_ENTER));
	eat_tok(parser);
	if (CUR_TOK(parser).kind != SCLEXER_EOL) {
		if (CUR_TOK(parser).kind != SCLEXER_KEYWORD)
			goto err_miss_block;
		return parse_keyword(parser);
	}
	eat_tok(parser);
	if (!eat_tok_with_kind(SCLEXER_INDENT_BLOCK_BEGIN, parser))
		goto err_miss_block;
	while ((ret = parse_tok(parser)) != PARSE_TOK_RESULT_END) {
		if (ret == PARSE_TOK_RESULT_ERR)
			return 1;
	}
	return 0;
err_miss_block:
	printf(ERR_FMT"miss block\n", ERR_FMT_ARG(parser));
	return 1;
}
