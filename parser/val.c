/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "include/expr.h"
#include "include/enum.h"
#include "include/lexer.h"
#include "include/struct.h"
#include "include/val.h"
#include "../include/array.h"
#include "../include/backend.h"
#include "../include/const.h"
#include <sclexer.h>

static int val_get_sym_elem(struct parser *parser, yz_val *val, str *s);
static int val_handle_sclexer_kind(yz_val *val, struct sclexer_tok *tok);
static int val_parse_expr(struct parser *parser, yz_val *val);
static int val_parse_func_call(struct parser *parser, yz_val *val,
		struct symbol *fn);
static int val_parse_int(struct sclexer_tok *tok, yz_val *val);
static int val_parse_str(struct parser *parser, yz_val *val);
static int val_parse_sym(struct parser *parser, yz_val *val, str *s);
static int val_parse_unary_expr(struct parser *parser, yz_val *val, char sym);
static int val_read_sym_from_mod(struct parser *parser, yz_val *val,
		yz_module *mod);

int val_get_sym_elem(struct parser *parser, yz_val *val, str *s)
{
	yz_module *mod = NULL;
	struct symbol *sym = NULL;
	struct lexer_tok tok;
	if (lexer_read_tok(&tok, &parser->lexer))
		return 1;
	if (tok.type != TOK_TYPE_DOT)
		return 1;
	if ((mod = parser_imported_find(&parser->imported, s)) != NULL)
		return val_read_sym_from_mod(parser, val, mod);
	if (!symbol_find(s, &sym, parser->scope, SYMG_SYM))
		return enum_read(parser, val, s);
	val->data.v = sym;
	val->type.type = AMC_SYM;
	val->type.v = val->data.v;
	return struct_get_elem(parser, val);
}

int val_handle_sclexer_kind(yz_val *val, struct sclexer_tok *tok)
{
	switch (tok->type) {
	case SCLEXER_TOK_TYPE_INT:
	case SCLEXER_TOK_TYPE_INT_NEG:
		return val_parse_int(tok, val);
	default: break;
	}
	return 1;
}

int val_parse_expr(struct parser *parser, yz_val *val)
{
	val->data.expr = parse_expr(parser, 0);
	val->type.type = AMC_EXPR;
	val->type.v = val->data.expr;
	return 0;
}

int val_parse_func_call(struct parser *parser, yz_val *val, struct symbol *fn)
{
	struct symbol *orig_sym = parser->sym;
	val->data.sym = fn;
	val->type.type = AMC_SYM;
	val->type.v = val->data.sym;
	parser->sym = fn;
	if (fn->parse_function(parser))
		return 1;
	parser->sym = orig_sym;
	return 0;
}

int val_parse_int(struct sclexer_tok *tok, yz_val *val)
{
	val->type.type = yz_get_int_size(tok->type_data.sint);
	val->type.v = NULL;
	val->data.l = tok->type_data.sint;
	return 0;
}

int val_parse_str(struct parser *parser, yz_val *val)
{
	yz_array_type *arr;
	yz_const *c;
	str *s;
	struct sclexer_str_slice slice;
	if (sclexer_read_to(&slice, &parser->lexer, "\""))
		return 1;
	s = str_new();
	if ((s->s = str2chr(slice.s, slice.len)) == NULL)
		goto err_free_s;
	c = calloc(1, sizeof(*c));
	if (backend_call(const_def_str)(&c->be_data, s))
		goto err_free_s_and_c;
	s->len = slice.len;
	arr = calloc(1, sizeof(*arr));
	arr->len = slice.len;
	arr->type.type = YZ_CHAR;
	c->val.type.type = YZ_ARRAY;
	c->val.type.v = arr;
	val->type.type = YZ_CONST;
	val->type.v = &c->val.type;
	val->data.v = c;
	return 0;
err_free_s_and_c:
	free(c);
err_free_s:
	str_free(s);
	return 0;
}

int val_parse_sym(struct parser *parser, yz_val *val, str *s)
{
	char *err_msg;
	struct symbol *sym = NULL;
	if (parser->lexer.cur[0] == '.')
		return val_get_sym_elem(parser, val, s);
	if (symbol_find(s, &sym, parser->scope, SYMG_FUNC))
		return val_parse_func_call(parser, val, sym);
	if (!symbol_find(s, &sym, parser->scope, SYMG_SYM))
		goto err_ident_not_found;
	val->data.sym = sym;
	val->type.type = AMC_SYM;
	val->type.v = val->data.v;
	return 0;
err_ident_not_found:
	err_msg = str2chr(s->s, s->len);
	printf(LEXER_ERR_FMT"Identifier: '%s' not found!\n",
			LEXER_ERR_FMT_ARG(parser->lexer),
			err_msg);
	free(err_msg);
	return 1;
}

int val_parse_unary_expr(struct parser *parser, yz_val *val, char sym)
{
	val->data.expr = parse_expr_unary(parser, sym);
	if (val->data.expr == NULL)
		return 1;
	val->type.type = AMC_EXPR;
	val->type.v = val->data.expr;
	return 0;
}

int val_read_sym_from_mod(struct parser *parser, yz_val *val, yz_module *mod)
{
	struct scope *orig_scope = parser->scope;
	int ret = 0;
	struct symbol *sym = NULL;
	struct lexer_tok tok;
	if (lexer_read_tok(&tok, &parser->lexer))
		return 1;
	if (tok.type != TOK_TYPE_STR)
		return 1;
	parser->scope = mod->scope;
	ret = symbol_find(&tok.data.s, &sym, parser->scope, SYMG_FUNC);
	parser->scope = orig_scope;
	if (!ret)
		return 1;
	return val_parse_func_call(parser, val, sym);
}

int parse_val(struct parser *parser, yz_val *val)
{
	struct lexer_tok tok;
	if (lexer_read_tok(&tok, &parser->lexer))
		return 1;
	switch (tok.type) {
	case TOK_TYPE_SCLEXER:
		return val_handle_sclexer_kind(val, &tok.self);
	case TOK_TYPE_STR:
		return val_parse_sym(parser, val, &tok.data.s);
	case TOK_TYPE_PAREN_L:
		return val_parse_expr(parser, val);
	case TOK_TYPE_DOUBLE_QUOTATION:
		return val_parse_str(parser, val);
	case TOK_TYPE_OP_MUL:
		return val_parse_unary_expr(parser, val, '*');
	case TOK_TYPE_OP_GET_ADDR:
		return val_parse_unary_expr(parser, val, '&');
	default: break;
	}
	return 1;
}
