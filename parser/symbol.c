/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "include/array.h"
#include "include/enum.h"
#include "include/func.h"
#include "include/struct.h"
#include "include/symbol.h"
#include "include/token.h"
#include "../include/backend.h"
#include "../include/ptr.h"
#include "../include/symbol.h"
#include "../include/token.h"
#include <stdio.h>

static int symbol_read_from_mod(struct parser *parser, yz_val *val,
		yz_module *mod);
static int symbol_read_get_elem(struct parser *parser, yz_val *val,
		str *token);

int symbol_read_from_mod(struct parser *parser, yz_val *val, yz_module *mod)
{
	struct scope *orig_scope = parser->scope;
	int ret = 0;
	struct symbol *sym = NULL;
	str token = TOKEN_NEW;
	file_pos_next(parser->f);
	file_skip_space(parser->f);
	if (token_read_before(SPECIAL_TOKEN_END, &token, parser->f) == NULL)
		return 1;
	file_skip_space(parser->f);
	parser->scope = mod->scope;
	ret = symbol_find(&token, &sym, parser->scope, SYMG_FUNC);
	parser->scope = orig_scope;
	if (!ret)
		return 1;
	return func_call_read(parser, val, sym);
}

int symbol_read_get_elem(struct parser *parser, yz_val *val, str *token)
{
	yz_module *mod = NULL;
	struct symbol *sym = NULL;
	if ((mod = parser_imported_find(&parser->imported, token)) != NULL)
		return symbol_read_from_mod(parser, val, mod);
	if (!symbol_find(token, &sym, parser->scope, SYMG_SYM))
		return enum_read(parser, val, token);
	val->data.v = sym;
	val->type.type = AMC_SYM;
	val->type.v = val->data.v;
	if (sym->result_type.type == YZ_PTR
			&& ((yz_ptr_type*)sym->result_type.v)
			->ref.type == YZ_STRUCT)
		return struct_get_elem_from_ptr(parser, val);
	if (sym->result_type.type != YZ_STRUCT)
		goto err_syntax_err;
	return struct_get_elem(parser, val);
err_syntax_err:
	backend_stop(BE_STOP_SIGNAL_ERR);
	return 1;
}

int symbol_read(struct parser *parser, yz_val *val)
{
	char *err_msg;
	struct symbol *sym = NULL;
	str token = TOKEN_NEW;
	if (token_read_before(SPECIAL_TOKEN_END, &token, parser->f) == NULL)
		return 1;
	file_skip_space(parser->f);
	if (parser->f->src[parser->f->pos] == '.')
		return symbol_read_get_elem(parser, val, &token);
	if (symbol_find(&token, &sym, parser->scope, SYMG_FUNC))
		return func_call_read(parser, val, sym);
	if (!symbol_find(&token, &sym, parser->scope, SYMG_SYM))
		goto err_identifier_not_found;
	val->data.v = sym;
	val->type.type = AMC_SYM;
	val->type.v = val->data.v;
	if (parser->f->src[parser->f->pos] == '[')
		return array_get_elem(parser, val);
	file_skip_space(parser->f);
	return 0;
err_identifier_not_found:
	err_msg = str2chr(token.s, token.len);
	printf("amc: %s: %lld,%lld: Identifier: '%s' not found!\n",
			__func__,
			parser->f->cur_line, parser->f->cur_column,
			err_msg);
	free(err_msg);
	backend_stop(BE_STOP_SIGNAL_ERR);
	return 1;
}

int symbol_read_name(str *result, struct file *f)
{
	if (result == NULL || f == NULL)
		return 1;
	result->s = &f->src[f->pos];
	if (isdigit(f->src[f->pos]))
		goto err_start_by_digit;
	for (; is_sym_chr(f->src[f->pos]); result->len++)
		file_pos_next(f);
	if (f->src[f->pos] != ':' && !isspace(f->src[f->pos]))
		goto err_invaild_chr;
	file_skip_space(f);
	return 0;
err_start_by_digit:
	printf("amc: %s: %lld,%lld: "ERROR_STR":\n"
			"| Symbol name start by digit: '%c'\n",
			__func__, f->cur_line, f->cur_column, f->src[f->pos]);
	return 1;
err_invaild_chr:
	printf("amc: %s: %lld,%lld: "ERROR_STR":\n"
			"| Invaild character: '%c'\n",
			__func__, f->cur_line, f->cur_column, f->src[f->pos]);
	return 1;
}
