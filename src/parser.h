/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef AMC_PARSER_H
#define AMC_PARSER_H
#include "fn.h"
#include "lexer.h"
#include "scope.h"
#include "symbol.h"
#include <mcb/mcb.h>
#include <sclexer.h>
#include <stdbool.h>
#include <stddef.h>

#define CUR_TOK(PARSER) ((PARSER)->tokens[(PARSER)->cur_token])
#define CUR_TOK_IS_SYM(PARSER, SYM) \
	(CUR_TOK(PARSER).kind == SCLEXER_SYMBOL \
	 && CUR_TOK(PARSER).data.symbol == SYM)
#define ERR_FMT LOC_FMT "\x1b[31merror\x1b[0m: "
#define ERR_FMT_ARG(PARSER) LOC_FMT_ARG(PARSER)

#define LOC_FMT "\x1b[1m%s\x1b[0m:%lu:%lu: "
#define LOC_FMT_ARG(PARSER) \
	CUR_TOK(PARSER).loc.fpath, \
	CUR_TOK(PARSER).loc.line, \
	CUR_TOK(PARSER).loc.column

enum PARSER_LIST_RESULT {
	LIST_CONTINUE,
	LIST_END,
	LIST_ERR
};

struct parser {
	size_t cur_token;
	struct sclexer_tok *tokens;
	size_t tokens_count;

	struct mcb_context mcb;

	struct yz_func *cur_func;
	struct yz_scope *cur_scope;
	struct yz_module *cur_mod; /* for public */
	struct yz_module **imported_mod;
	size_t imported_mod_count;
	struct yz_symbol symbols; /* private */
};

void eat_tok(struct parser *parser);
bool eat_tok_with_kind(enum SCLEXER_TOK_KIND kind, struct parser *parser);
bool eat_tok_with_sym(enum LEXER_SYMBOLS sym, struct parser *parser);
struct yz_symbol *find_symbol_in_parser(const struct parser *parser,
		const char *name,
		size_t name_len);
void parser_init(struct parser *parser, struct yz_module *mod);
int parse_file(struct parser *parser, const char *fpath);
struct sclexer_tok *peek_tok(struct parser *parser, size_t offset);
struct sclexer_tok *peek_tok_with_kind(enum SCLEXER_TOK_KIND kind,
		struct parser *parser,
		size_t offset);
struct sclexer_tok *peek_tok_with_sym(enum LEXER_SYMBOLS sym,
		struct parser *parser,
		size_t offset);

#endif
