/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "include/block.h"
#include "include/expr.h"
#include "include/indent.h"
#include "include/lexer.h"
#include "include/keywords.h"
#include "include/utils.h"
#include "../include/backend.h"
#include "../include/parser.h"
#include "../include/scope.h"
#include <sclexer.h>
#include <stdio.h>

static int block_parse_direct(struct parser *parser);
static int block_parse_expr(struct parser *parser);
static int block_parse_keyword(struct parser *parser);
static int block_parse_line(struct parser *parser);

int block_parse_direct(struct parser *parser)
{
	int ret = 0;
	if ((ret = block_parse_keyword(parser)) == 0)
		return 0;
	if (ret > 0)
		return 1;
	return block_parse_expr(parser);
}

int block_parse_expr(struct parser *parser)
{
	struct expr *expr = NULL;
	if ((expr = parse_expr(parser, 1)) == NULL)
		return err_print_pos(__func__, "Cannot parse expression!",
				parser->lexer.line, parser->lexer.column);
	if (expr_apply(parser, expr) > 0)
		return err_print_pos(__func__, "Cannot apply expression!",
				parser->lexer.line, parser->lexer.column);
	free_expr(expr);
	return 0;
}

int block_parse_keyword(struct parser *parser)
{
	struct sclexer lexer_record;
	long orig_pos = sclexer_record(&parser->lexer, &lexer_record);
	struct symbol *sym = NULL;
	struct lexer_tok tok;
	if (orig_pos == -1)
		return 1;
	if (lexer_read_tok(&tok, &parser->lexer))
		return 1;
	if (tok.type != TOK_TYPE_STR)
		return 1;
	if (!keyword_find(&tok.data.s, &sym)) {
		if (sclexer_restore(&parser->lexer, &lexer_record, orig_pos))
			return 1;
		return -1;
	}
	if (!sym->flags.in_block)
		goto err_not_in_block;
	parser->sym = sym;
	return sym->parse_function(parser);
err_not_in_block:
	printf("amc: block_parse_keyword: Symbol unsupport used in block!\n");
	backend_stop(BE_STOP_SIGNAL_ERR);
	return 1;
}

int block_parse_line(struct parser *parser)
{
	struct sclexer lexer_record;
	long orig_pos = sclexer_record(&parser->lexer, &lexer_record);
	if (orig_pos == -1)
		return LEXER_RESULT_FAULT;
	if (sclexer_get_line(&parser->lexer) == EOF)
		return LEXER_RESULT_FAULT;
	if (indent_read(&parser->lexer) != parser->scope->indent) {
		if (sclexer_restore(&parser->lexer, &lexer_record, orig_pos))
			return LEXER_RESULT_FAULT;
		return LEXER_RESULT_END;
	}
	if (block_parse_direct(parser))
		return LEXER_RESULT_FAULT;
	return LEXER_RESULT_CONTINUE;
}

int parse_block(struct parser *parser)
{
	int ret = 0;
	struct scope cur_scope = {
		.fn = parser->scope->fn,
		.indent = parser->scope->indent + 1,
		.parent = parser->scope,
		.status = backend_call(scope_begin)()
	};
	if (scope_check_is_correct(&cur_scope))
		return 1;
	parser->scope = &cur_scope;
	if (parser->lexer.cur[0] != '\n') {
		if (block_parse_direct(parser))
			goto err_restore_scope;
		goto restore;
	}
	while ((ret = block_parse_line(parser)) != LEXER_RESULT_END) {
		if (ret == LEXER_RESULT_FAULT)
			goto err_restore_scope;
	}
restore:
	parser->scope = cur_scope.parent;
	scope_end(&cur_scope);
	return 0;
err_restore_scope:
	parser->scope = cur_scope.parent;
	scope_end(&cur_scope);
	return 1;
}
