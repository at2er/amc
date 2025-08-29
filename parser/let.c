/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "include/array.h"
#include "include/identifier.h"
#include "include/keywords.h"
#include "include/lexer.h"
#include "include/struct.h"
#include "include/type.h"
#include "../include/backend.h"
#include "../include/parser.h"

static int let_init_constructor(struct parser *parser, struct symbol *sym);
static int let_init_val(struct parser *parser, struct symbol *sym);
static int let_reg_sym(struct parser *parser, struct symbol *sym);

int let_init_constructor(struct parser *parser, struct symbol *sym)
{
	struct lexer_tok tok;
	if (lexer_read_tok(&tok, &parser->lexer)
			|| tok.type != TOK_TYPE_BRACE_L)
		return 1;
	switch (sym->result_type.type) {
	case YZ_ARRAY:
		return constructor_array(parser, sym);
		break;
	case YZ_STRUCT:
		return constructor_struct(parser, sym);
		break;
	default:
		return 1;
		break;
	}
	return 0;
}

int let_init_val(struct parser *parser, struct symbol *sym)
{
	if (parser->lexer.cur[0] == '{')
		return let_init_constructor(parser, sym);
	if (identifier_assign_val(parser, sym, OP_ASSIGN))
		return 1;
	return 0;
}

int let_reg_sym(struct parser *parser, struct symbol *sym)
{
	sym->argc = 1;
	if (symbol_register(sym, &parser->scope->sym_groups[SYMG_SYM]))
		goto err_cannot_register_sym;
	return 0;
err_cannot_register_sym:
	printf(LEXER_ERR_FMT"Cannot register symbol!\n",
			LEXER_ERR_FMT_ARG(parser->lexer));
	backend_stop(BE_STOP_SIGNAL_ERR);
	return 1;
}

int parse_let(struct parser *parser)
{
	struct symbol *result = calloc(1, sizeof(*result));
	struct lexer_tok tok;
	result->type = SYM_IDENTIFIER;
	result->flags.mut = identifier_check_mut(&parser->lexer);
	if (parse_type_name_pair(parser, &result->name, &result->result_type))
		goto err_free_result;
	if (let_reg_sym(parser, result))
		goto err_free_result;
	if (lexer_read_tok(&tok, &parser->lexer)
			|| tok.type != TOK_TYPE_OP_ASSIGN)
		goto err_syntax_err;
	return let_init_val(parser, result);
err_syntax_err:
	printf(LEXER_ERR_FMT"Syntax error!\n",
			LEXER_ERR_FMT_ARG(parser->lexer));
err_free_result:
	free_symbol(result);
	backend_stop(BE_STOP_SIGNAL_ERR);
	return 1;
}
