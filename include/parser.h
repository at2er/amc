/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef AMC_PARSER_H
#define AMC_PARSER_H
#include "lexer.h"
#include "scope.h"
#include <mcb/mcb.h>

#define CUR_TOK(PARSER) ((PARSER)->lexer.cur_block->begin)

enum LIST_PARSER_RESULT {
	LIST_CONTINUE,
	LIST_END,
	LIST_ERR
};

struct parser {
	struct lexer lexer;

	struct mcb_context *mcb;

	struct yz_scope *cur_scope;
	struct yz_module *cur_mod;
	struct yz_module **imported_mod;
	uint32_t imported_mod_count;
	struct yz_symbol *symbols[UCHAR_MAX + 1];
};

void parser_init(struct parser *parser);
int parse_file(struct parser *parser, const char *fpath);

#endif
