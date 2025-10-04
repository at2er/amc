/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "die.h"
#include "lexer.h"
#include "panic.h"
#include "parser.h"
#include "parser/fn.h"
#include <assert.h>
#include <memory.h>

static int parse_keyword(struct parser *parser);

int parse_keyword(struct parser *parser)
{
	assert(CUR_TOK(parser)->type == TOK_KEYWORD);
	switch (CUR_TOK(parser)->data.keyword) {
	case KEYWORD_FN:
		lexer_eat_tok(parser->lexer.cur_block);
		return parse_func_def(parser, YZ_SCOPE_PRIVATE);
	default: break;
	}
	printf(PANIC_FMT"unsupport keyword: '%s'\n", PANIC_FMT_ARG,
			keyword_get_str(CUR_TOK(parser)
				->data.keyword));
	return 1;
}

void parser_init(struct parser *parser)
{
	parser->cur_scope = NULL;
	parser->imported_mod = NULL;
	parser->imported_mod_count = 0;
	memset(&parser->symbols, '\0', sizeof(parser->symbols));
	if (mcb_init(parser->mcb, MCB_MODE_GNU_ASM))
		die(PANIC_FMT"failed to init mcb\n", PANIC_FMT_ARG);
}

int parse_file(struct parser *parser, const char *fpath)
{
	assert(parser && fpath);
	lexer_init(&parser->lexer);
	if (lexer_parse_file(&parser->lexer, fpath))
		return 1;
	while (CUR_TOK(parser)->type != TOK_EOF) {
		switch (CUR_TOK(parser)->type) {
		case TOK_KEYWORD:
			if (parse_keyword(parser))
				return 1;
			continue;
		default: break;
		}
	}
	if (mcb_done(parser->mcb, stdout))
		die(PANIC_FMT"failed to make mcb done\n", PANIC_FMT_ARG);
	return 0;
}
