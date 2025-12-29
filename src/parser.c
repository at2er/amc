/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "compiler/out.h"
#include "die.h"
#include "keyword.h"
#include "lexer.h"
#include "module.h"
#include "panic.h"
#include "parser.h"
#include "parser/fn.h"
#include "parser/pub.h"
#include "symbol.h"
#include <assert.h>
#include <mcb/mcb.h>
#include <memory.h>
#include <sclexer.h>
#include <sctrie.h>
#include <stdbool.h>
#include <stdlib.h>

static int parse_tokens(struct parser *parser);
static int parse_keyword(struct parser *parser);

int parse_tokens(struct parser *parser)
{
	while (parser->cur_token < parser->tokens_count) {
		switch (CUR_TOK(parser).kind) {
		case SCLEXER_KEYWORD:
			if (parse_keyword(parser))
				return 1;
			continue;
		case SCLEXER_EOL:
			eat_tok(parser);
			continue;
		default: goto panic_unsupport_tok; break;
		}
	}
	return 0;
panic_unsupport_tok:
	panicf(LOC_FMT"failed to parse token '%s'",
			LOC_FMT_ARG(parser),
			get_token_str(&CUR_TOK(parser)));
	return 1;
}

int parse_keyword(struct parser *parser)
{
	assert(CUR_TOK(parser).kind == SCLEXER_KEYWORD);
	switch (CUR_TOK(parser).data.keyword) {
	case KEYWORD_FN:
		eat_tok(parser);
		return parse_func_def(parser, YZ_SCOPE_PRIVATE);
	case KEYWORD_PUB:
		eat_tok(parser);
		return parse_pub(parser);
	default: break;
	}
	die(LOC_FMT PANIC_FMT"unsupport keyword: '%s'\n",
			LOC_FMT_ARG(parser), PANIC_FMT_ARG,
			get_token_str(&CUR_TOK(parser)));
	return 1;
}

void eat_tok(struct parser *parser)
{
	parser->cur_token++;
}

bool eat_tok_with_kind(enum SCLEXER_TOK_KIND kind, struct parser *parser)
{
	if (CUR_TOK(parser).kind != kind)
		return false;
	eat_tok(parser);
	return true;
}

bool eat_tok_with_sym(enum LEXER_SYMBOLS sym, struct parser *parser)
{
	if (!CUR_TOK_IS_SYM(parser, sym))
		return false;
	eat_tok(parser);
	return true;
}

struct yz_symbol *find_symbol_in_parser(const struct parser *parser,
		const char *name,
		size_t name_len)
{
	struct yz_symbol *result = sctrie_find_elem(&parser->symbols,
			name, name_len);
	if (result)
		return result;
	return sctrie_find_elem(&parser->cur_mod->symbols, name, name_len);
}

void init_parser(struct parser *parser,
		struct yz_module *mod,
		struct mcb_context *mcb)
{
	assert(parser && mod);
	memset(parser, 0, sizeof(*parser));
	parser->cur_mod = mod;
	parser->mcb = mcb;
}

int parse_file(struct parser *parser, const char *fpath)
{
	struct sclexer lexer = {0};
	char *src = NULL;

	assert(parser && fpath);

	lexer.src_siz = sclexer_read_file(&src, fpath);
	lexer.src = src;
	init_lexer(&lexer, fpath);

	parser->tokens_count = sclexer_get_tokens(&lexer, &parser->tokens);
	print_tokens(&lexer, parser->tokens, parser->tokens_count);

	if (parse_tokens(parser))
		return 1;

	free(src);

	return gen_file_output(parser->mcb, fpath);
}

struct sclexer_tok *peek_tok(struct parser *parser, size_t offset)
{
	if (parser->tokens_count - parser->cur_token < offset)
		return NULL;
	return &parser->tokens[parser->cur_token + offset];
}

struct sclexer_tok *peek_tok_with_kind(enum SCLEXER_TOK_KIND kind,
		struct parser *parser,
		size_t offset)
{
	struct sclexer_tok *t = peek_tok(parser, offset);
	if (t->kind == kind)
		return t;
	return NULL;
}

struct sclexer_tok *peek_tok_with_sym(enum LEXER_SYMBOLS sym,
		struct parser *parser,
		size_t offset)
{
	struct sclexer_tok *t = peek_tok(parser, offset);
	if (TOK_IS_SYM(t, sym))
		return t;
	return NULL;
}
