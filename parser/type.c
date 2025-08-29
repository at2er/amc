/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "include/array.h"
#include "include/lexer.h"
#include "include/ptr.h"
#include "include/type.h"
#include "../include/parser.h"
#include <sctrie.h>
#include <stdio.h>
#include <string.h>

static int parse_type_handle_str_kind(struct parser *parser, yz_type *result,
		str *s);
static int type_get_from_module(str *name, yz_type *type,
		struct parser *parser);
static int type_pair_parse_type(struct parser *parser, yz_type *type);

int parse_type_handle_str_kind(struct parser *parser, yz_type *result, str *s)
{
	yz_user_type *type = NULL;
	if (parser->lexer.cur[0] == '.') {
		if (type_get_from_module(s, result, parser))
			return 1;
		return 0;
	}
	result->type = yz_type_get(s);
	result->v = NULL;
	if (result->type != AMC_ERR_TYPE)
		return 0;
	if ((type = yz_user_type_find(s, parser->scope)) == NULL)
		return 1;
	return parse_type_user(type, result);
}

int type_get_from_module(str *name, yz_type *type, struct parser *parser)
{
	yz_module *mod = NULL;
	struct scope *orig_scope = parser->scope;
	int ret = 0;
	struct lexer_tok tok;
	if (lexer_read_tok(&tok, &parser->lexer))
		return 1;
	if (tok.type != TOK_TYPE_DOT)
		return 1;
	if ((mod = parser_imported_find(&parser->imported, name)) == NULL)
		return 1;
	parser->scope = mod->scope;
	ret = parse_type(parser, type);
	parser->scope = orig_scope;
	return ret;
}

int type_pair_parse_type(struct parser *parser, yz_type *type)
{
	struct lexer_tok tok;
	if (lexer_read_tok(&tok, &parser->lexer))
		return 1;
	if (tok.type != TOK_TYPE_COLON)
		goto err_type_indicator_not_found;
	return parse_type(parser, type);
err_type_indicator_not_found:
	printf(LEXER_ERR_FMT"Type indicator not found!\n",
			LEXER_ERR_FMT_ARG(parser->lexer));
	return 1;
}

int parse_type(struct parser *parser, yz_type *result)
{
	struct lexer_tok tok;
	if (result == NULL)
		return 1;
	if (lexer_read_tok(&tok, &parser->lexer))
		return 1;
	switch (tok.type) {
	case TOK_TYPE_STR:
		return parse_type_handle_str_kind(parser, result, &tok.data.s);
	case TOK_TYPE_OP_MUL:
		return parse_type_ptr(parser, result);
	case TOK_TYPE_BRACKET_L:
		return parse_type_array(parser, result);
	default: break;
	}
#ifdef DEBUG
	printf(LEXER_ERR_FMT"Token type not handled\n",
			LEXER_ERR_FMT_ARG(parser->lexer));
#endif
	return 1;
}

int parse_type_name_pair(struct parser *parser, str *name, yz_type *type)
{
	struct lexer_tok tok;
	if (lexer_read_tok(&tok, &parser->lexer))
		return 1;
	if (tok.type != TOK_TYPE_STR)
		return 1;
	str_copy(&tok.data.s, name);
	return type_pair_parse_type(parser, type);
}

int parse_type_user(yz_user_type *type, yz_type *result)
{
	switch (type->type) {
	case YZ_STRUCT:
		result->type = YZ_STRUCT;
		result->v = type->data.struct_;
		break;
	case YZ_ENUM:
		result->type = YZ_ENUM;
		result->v = type->data.enum_;
		break;
	default: goto err_unsupport; break;
	}
	return 0;
err_unsupport:
	printf("amc: parse_type_user: Unsupport type: '%s':%d\n",
			yz_get_raw_type_name(type->type),
			type->type);
	return 1;
}

yz_user_type *yz_user_type_find(str *s, struct scope *scope)
{
	yz_user_type *result = sctrie_find_elem(&scope->types, s->s, s->len);
	if (result != NULL)
		return result;
	if (scope->parent != NULL)
		return yz_user_type_find(s, scope->parent);
	return NULL;
}
