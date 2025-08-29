/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "include/enum.h"
#include "include/keywords.h"
#include "include/lexer.h"
#include "include/type.h"
#include "../include/backend.h"
#include "../include/parser.h"
#include "../utils/utils.h"
#include <sctrie.h>
#include <stdio.h>
#include <string.h>

static int enum_def_multi_line(yz_enum *self, struct parser *parser);
static int enum_def_read_item(yz_enum *self, struct sclexer *lexer);
static int enum_def_reg(yz_enum *self, struct scope *scope);
static int enum_def_reg_item(yz_enum *self, yz_enum_item *item);
static int enum_def_single_line(yz_enum *self, struct parser *parser);

int enum_def_multi_line(yz_enum *self, struct parser *parser)
{
	struct sclexer lexer_record;
	uint64_t orig_pos;
	struct lexer_tok tok;
	if (lexer_read_tok(&tok, &parser->lexer))
		return 1;
	if (tok.type != TOK_TYPE_NEXT_LINE)
		return 1;
	if ((orig_pos = sclexer_record(&parser->lexer, &lexer_record)) == -1)
		return 1;
	while (!sclexer_get_line(&parser->lexer)) {
		if (lexer_read_tok(&tok, &parser->lexer))
			return 1;
		if (tok.type != TOK_TYPE_PIPE_LINE)
			break;
		if (enum_def_single_line(self, parser))
			return 1;
		orig_pos = sclexer_record(&parser->lexer, &lexer_record);
		if (orig_pos == -1)
			return 1;
	}
	return sclexer_restore(&parser->lexer, &lexer_record, orig_pos);
}

int enum_def_read_item(yz_enum *self, struct sclexer *lexer)
{
	yz_enum_item *item = calloc(1, sizeof(*item));
	struct lexer_tok tok;
	if (lexer_read_tok(&tok, lexer))
		return LEXER_RESULT_FAULT;
	if (tok.type != TOK_TYPE_STR)
		return LEXER_RESULT_FAULT;
	str_copy(&tok.data.s, &item->name);
	item->data.u = self->count;
	if (enum_def_reg_item(self, item))
		return LEXER_RESULT_FAULT;
	if (lexer_read_tok(&tok, lexer))
		return LEXER_RESULT_FAULT;
	if (tok.type == TOK_TYPE_NEXT_LINE)
		return LEXER_RESULT_END;
	if (tok.type == TOK_TYPE_PIPE_LINE)
		return LEXER_RESULT_CONTINUE;
	return LEXER_RESULT_FAULT;
}

int enum_def_reg(yz_enum *self, struct scope *scope)
{
	yz_user_type *type = sctrie_append_elem(&scope->types, sizeof(*type),
			self->name.s, self->name.len);
	if (type == NULL)
		goto err_defined;
	type->type = YZ_ENUM;
	type->data.enum_ = self;
	return 0;
err_defined:
	printf("amc: enum_def_reg: "
			"Type defined: '%s'\n", self->name.s);
	backend_stop(BE_STOP_SIGNAL_ERR);
	return 1;
}

int enum_def_reg_item(yz_enum *self, yz_enum_item *item)
{
	self->count += 1;
	self->elems = realloc(self->elems, sizeof(*self->elems) * self->count);
	self->elems[self->count - 1] = item;
	return 0;
}

int enum_def_single_line(yz_enum *self, struct parser *parser)
{
	int ret = 0;
	while ((ret = enum_def_read_item(self, &parser->lexer))
			!= LEXER_RESULT_END) {
		if (ret == LEXER_RESULT_FAULT)
			return 1;
	}
	return 0;
}

int enum_read(struct parser *parser, yz_val *val, str *name)
{
	char *err_msg;
	yz_enum_item *item = NULL;
	yz_enum *src = yz_enum_find(name, parser->scope);
	struct lexer_tok tok;
	if (src == NULL)
		goto err_enum_not_found;
	if (lexer_read_tok(&tok, &parser->lexer))
		return 1;
	if (tok.type != TOK_TYPE_STR)
		return 1;
	if ((item = yz_enum_item_find(&tok.data.s, src)) == NULL)
		return 1;
	val->type.type = YZ_ENUM_ITEM;
	val->type.v = src;
	val->data.l = item->data.u;
	return 0;
err_enum_not_found:
	err_msg = str2chr(name->s, name->len);
	printf(LEXER_ERR_FMT"Enum: '%s' not found!\n",
			LEXER_ERR_FMT_ARG(parser->lexer),
			err_msg);
	free(err_msg);
	return 1;
}

yz_enum *yz_enum_find(str *s, struct scope *scope)
{
	struct yz_user_type *type = yz_user_type_find(s, scope);
	if (!type || type->type != YZ_ENUM)
		return NULL;
	return type->data.enum_;
}

yz_enum_item *yz_enum_item_find(str *s, yz_enum *src)
{
	for (int i = 0; i < src->count; i++) {
		if (s->len != src->elems[i]->name.len)
			continue;
		if (strncmp(s->s, src->elems[i]->name.s, s->len) == 0)
			return src->elems[i];
	}
	return NULL;
}

int parse_enum(struct parser *parser)
{
	str name = STR_EMPTY;
	yz_enum *self = calloc(1, sizeof(*self));
	struct lexer_tok tok;
	if (parse_type_name_pair(parser, &name, &self->type))
		goto err_free_self;
	if (lexer_read_tok(&tok, &parser->lexer))
		goto err_free_self;
	if (tok.type != TOK_TYPE_BLOCK_START)
		goto err_block_not_start;
	self->name.s = name.s;
	self->name.len = name.len;
	if (!YZ_IS_DIGIT(self->type.type))
		goto err_not_digit;
	if (enum_def_reg(self, parser->scope))
		goto err_free_self;
	if (parser->lexer.cur[0] != '\n') {
		if (enum_def_single_line(self, parser))
			goto err_free_self;
		return 0;
	}
	if (enum_def_multi_line(self, parser))
		goto err_free_self;
	return 0;
err_not_digit:
	printf(LEXER_ERR_FMT ERROR_STR": "
			"Enum can only be based on digit: '%s'\n"
			"| "HINT_STR": use 'i8' to 'i64', "
			"'u8' to 'u64' or 'char'.\n",
			LEXER_ERR_FMT_ARG(parser->lexer),
			self->name.s);
	backend_stop(BE_STOP_SIGNAL_ERR);
	return 1;
err_block_not_start:
	printf(LEXER_ERR_FMT"Block not start!\n",
			LEXER_ERR_FMT_ARG(parser->lexer));
err_free_self:
	free_yz_enum(self);
	return 1;
}
