/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "lexer.h"
#include "panic.h"
#include "parser.h"
#include "parser/block.h"
#include "parser/return.h"
#include <assert.h>

enum PARSE_TOK_RESULT {
	PARSE_TOK_RESULT_CONTINUE,
	PARSE_TOK_RESULT_END,
	PARSE_TOK_RESULT_ERR
};

static int parse_keyword(struct parser *parser);
static enum PARSE_TOK_RESULT parse_tok(struct parser *parser);

int parse_keyword(struct parser *parser)
{
	assert(CUR_TOK(parser)->type == TOK_KEYWORD);
	switch (CUR_TOK(parser)->data.keyword) {
	case KEYWORD_RETURN:
		lexer_eat_tok(parser->lexer.cur_block);
		return parse_return(parser);
	default: break;
	}
	printf(PANIC_FMT"unsupport keyword: '%s'\n", PANIC_FMT_ARG,
			keyword_get_str(CUR_TOK(parser)
				->data.keyword));
	return 1;
}

enum PARSE_TOK_RESULT parse_tok(struct parser *parser)
{
	switch (CUR_TOK(parser)->type) {
	case TOK_EOF:
		return PARSE_TOK_RESULT_END;
	case TOK_EOL:
		lexer_eat_tok(parser->lexer.cur_block);
		if (CUR_TOK(parser)->type == TOK_EOB) {
			lexer_eat_tok(parser->lexer.cur_block);
			return PARSE_TOK_RESULT_END;
		}
		break;
	case TOK_KEYWORD:
		if (parse_keyword(parser))
			return PARSE_TOK_RESULT_ERR;
		return PARSE_TOK_RESULT_CONTINUE;
	default: break;
	}
	return PARSE_TOK_RESULT_CONTINUE;
}

int parse_block(struct parser *parser)
{
	struct lexer_block *orig_blk = parser->lexer.cur_block;
	enum PARSE_TOK_RESULT ret;
	assert(CUR_TOK(parser)->type == TOK_BLOCK_START);
	parser->lexer.cur_block = &CUR_TOK(parser)->data.block;
	while ((ret = parse_tok(parser)) != PARSE_TOK_RESULT_END) {
		if (ret == PARSE_TOK_RESULT_ERR)
			goto err_end;
	}
	parser->lexer.cur_block = orig_blk;
	return 0;
err_end:
	parser->lexer.cur_block = orig_blk;
	return 1;
}
