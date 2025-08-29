/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "include/block.h"
#include "include/expr.h"
#include "include/identifier.h"
#include "include/keywords.h"
#include "include/lexer.h"
#include "include/type.h"
#include "../include/backend.h"
#include "../include/parser.h"
#include "../include/scope.h"
#include "../include/checker/ptr.h"
#include "../utils/utils.h"
#include "include/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct func_call_handle {
	struct symbol *fn;
	int index;
	struct parser *parser;
	yz_val **vals;
};

static yz_val *func_call_arg_handle(struct expr *expr, struct symbol *arg);
static int func_call_main(struct parser *parser);
static int func_call_read_arg(struct func_call_handle *handle);
static int func_call_read_args(struct parser *parser, yz_val **result);
static int func_def_check_main(const char *name, int len);
static int func_def_end_scope(struct parser *parser,
		backend_func_def_handle *handle);
static int func_def_inherit_decorators(struct decorators *src,
		struct symbol *dest);
static int func_def_main(struct parser *parser);
static int func_def_read_arg(struct parser *parser);
static int func_def_read_args(struct parser *parser);
static int func_def_read_name(struct parser *parser);
static int func_def_read_type(struct parser *parser);
static yz_val *func_ret_get_val(struct symbol *fn, struct expr *expr);

yz_val *func_call_arg_handle(struct expr *expr, struct symbol *arg)
{
	yz_val *result = expr2yz_val(expr);
	if (result == NULL)
		return NULL;
	if ((result->type.type == AMC_SYM || result->type.type == YZ_NULL)
			&& arg->result_type.type == YZ_PTR) {
		if (!check_ptr_can_null(result, arg))
			goto err_free_result;
	}
	if (identifier_handle_val_type(&result->type, &arg->result_type))
		goto err_free_result;
	return result;
err_free_result:
	free_yz_val(result);
	return NULL;
}

int func_call_main(struct parser *parser)
{
	printf(LEXER_ERR_FMT ERROR_STR": "
			"You cannot call the main function!\n",
			LEXER_ERR_FMT_ARG(parser->lexer));
	backend_stop(BE_STOP_SIGNAL_ERR);
	return 1;
}

int func_call_read_arg(struct func_call_handle *handle)
{
	struct expr *expr = NULL;
	yz_val *result = NULL;
	struct lexer_tok tok;
	if (handle->index > handle->fn->argc)
		goto err_too_many_args;
	if ((expr = parse_expr(handle->parser, 1)) == NULL)
		goto err_print_pos;
	if (expr_apply(handle->parser, expr) > 0)
		goto err_print_pos;
	result = func_call_arg_handle(expr, handle->fn->args[handle->index]);
	if (result == NULL)
		goto err_print_pos;
	handle->vals[handle->index] = result;
	handle->index += 1;
	if (lexer_read_tok(&tok, &handle->parser->lexer))
		return LEXER_RESULT_FAULT;
	if (tok.type == TOK_TYPE_COLON)
		return LEXER_RESULT_CONTINUE;
	if (tok.type == TOK_TYPE_NEXT_LINE)
		return LEXER_RESULT_END;
	return LEXER_RESULT_FAULT;
err_too_many_args:
	printf(LEXER_ERR_FMT"Too many parameters.\n",
			LEXER_ERR_FMT_ARG(handle->parser->lexer));
	backend_stop(BE_STOP_SIGNAL_ERR);
	return LEXER_RESULT_FAULT;
err_print_pos:
	err_print_pos(__func__, NULL,
			handle->parser->lexer.line,
			handle->parser->lexer.column);
	return LEXER_RESULT_FAULT;
}

int func_call_read_args(struct parser *parser, yz_val **result)
{
	struct func_call_handle handle = {
		.fn = parser->sym,
		.index = 0,
		.parser = parser,
		.vals = result
	};
	int ret = 0;
	while ((ret = func_call_read_arg(&handle)) != LEXER_RESULT_END) {
		if (ret == LEXER_RESULT_FAULT)
			return 1;
	}
	if (handle.index < parser->sym->argc)
		goto err_too_few_arg;
	return 0;
err_too_few_arg:
	printf(LEXER_ERR_FMT"Too few arguments!\n"
			"| Function: \"%s\"\n"
			"| Need %d but only has %d\n",
			LEXER_ERR_FMT_ARG(parser->lexer),
			parser->sym->name.s,
			parser->sym->argc, handle.index);
	backend_stop(BE_STOP_SIGNAL_ERR);
	return 0;
}

int func_def_check_main(const char *name, int len)
{
	if (len != 4)
		return 0;
	if (strncmp(name, "main", len) == 0)
		return 1;
	return 0;
}

int func_def_end_scope(struct parser *parser, backend_func_def_handle *handle)
{
	struct scope *parent = parser->scope->parent;
	for (int i = 0; i < parser->scope->fn->argc; i++)
		parser->scope->sym_groups[SYMG_SYM].symbols[i] = NULL;
	if (handle != NULL)
		if (backend_call(func_def_end)(handle))
			return 1;
	scope_end(parser->scope);
	parser->scope = parent;
	return 0;
}

int func_def_inherit_decorators(struct decorators *src,
		struct symbol *dest)
{
	if ((dest->hooks = hooks_inherit(&src->hooks)) == NULL)
		return 0;
	src->used = 1;
	return 0;
}

int func_def_main(struct parser *parser)
{
	struct symbol *fn = parser->scope->fn;
	backend_func_def_handle *handle = NULL;
	if (!parser->stat.has_pub)
		goto err_not_pub;
	fn->result_type.type = YZ_I8;
	fn->result_type.v = NULL;
	global_parser.has_main = 1;
	if ((handle = backend_call(func_def)(fn, 1, 1)) == NULL)
		return 1;
	fn->parse_function = func_call_main;
	if (symbol_register(fn, &parser->scope_pub->sym_groups[SYMG_FUNC]))
		return 1;
	if (parse_block(parser))
		goto err_pop_symbol;
	return func_def_end_scope(parser, handle);
err_not_pub:
	printf("amc: func_def_main: "ERROR_STR":\n"
			"| Function: 'main' must be declared as 'pub'.\n");
	return 1;
err_pop_symbol:
	symbol_pop(&parser->scope_pub->sym_groups[SYMG_FUNC]);
	return 1;
}

int func_def_read_arg(struct parser *parser)
{
	struct symbol *sym = NULL;
	struct lexer_tok tok;
	sym = calloc(1, sizeof(*sym));
	if (parse_type_name_pair(parser, &sym->name, &sym->result_type))
		goto err_free_sym;
	sym->argc = parser->scope->fn->argc;
	sym->type = SYM_FUNC_ARG;
	if (symbol_register(sym, &parser->scope->sym_groups[SYMG_SYM]))
		goto err_free_sym;
	if (symbol_args_append(parser->scope->fn, sym))
		goto err_free_sym_and_remove;
	parser->scope->fn->argc++;
	if (lexer_read_tok(&tok, &parser->lexer))
		return LEXER_RESULT_FAULT;
	switch (tok.type) {
	case TOK_TYPE_COMMA:
		return LEXER_RESULT_CONTINUE;
	case TOK_TYPE_PAREN_R:
		return LEXER_RESULT_END;
	default: break;
	}
	return LEXER_RESULT_FAULT;
err_free_sym_and_remove:
	symbol_pop(&parser->scope->sym_groups[SYMG_SYM]);
err_free_sym:
	free_safe(sym);
	backend_stop(BE_STOP_SIGNAL_ERR);
	return LEXER_RESULT_FAULT;
}

int func_def_read_args(struct parser *parser)
{
	int ret = 0;
	struct lexer_tok tok;
	if (parser->lexer.cur[0] == ':')
		return 0;
	if (lexer_read_tok(&tok, &parser->lexer))
		return 1;
	if (tok.type != TOK_TYPE_PAREN_L)
		return 1;
	while ((ret = func_def_read_arg(parser)) != LEXER_RESULT_END) {
		if (ret == LEXER_RESULT_FAULT)
			return 1;
	}
	return 0;
}

int func_def_read_name(struct parser *parser)
{
	struct lexer_tok tok;
	if (lexer_read_tok(&tok, &parser->lexer))
		return 1;
	if (tok.type != TOK_TYPE_STR)
		return 1;
	if (str_copy(&tok.data.s, &parser->scope->fn->name))
		return 1;
	if (backend_call(symbol_get_path)(&parser->scope->fn->path,
				&parser->path,
				parser->scope->fn->name.s,
				parser->scope->fn->name.len))
		goto err_get_path_failed;
	return 0;
err_get_path_failed:
	printf("amc: func_def_read_name: Get symbol path failed!\n");
	backend_stop(BE_STOP_SIGNAL_ERR);
	return 1;
}

int func_def_read_type(struct parser *parser)
{
	struct lexer_tok tok;
	if (lexer_read_tok(&tok, &parser->lexer))
		return 1;
	if (tok.type != TOK_TYPE_COLON)
		return 1;
	if (parse_type(parser, &parser->scope->fn->result_type))
		return 1;
	return 0;
}

yz_val *func_ret_get_val(struct symbol *fn, struct expr *expr)
{
	yz_val *result = identifier_handle_expr_val(expr, &fn->result_type);
	if (result == NULL)
		return NULL;
	if (result->type.type == AMC_SYM) {
		if (fn->result_type.type == YZ_PTR)
			if (!check_ptr_can_ret(result->data.v, fn))
				goto err_free_result;
		return result;
	}
	return result;
err_free_result:
	free_yz_val(result);
	return NULL;
}

int parse_func_call(struct parser *parser)
{
	yz_val **args = NULL;
	struct symbol *fn = parser->sym;
	if (fn->argc != 0) {
		args = calloc(fn->argc, sizeof(*args));
		if (func_call_read_args(parser, args))
			goto err_free_args;
	}
	if (fn->hooks && hook_apply(parser, &fn->hooks
				->times[HOOK_FUNC_CALL_BEFORE]))
		return 1;
	if (backend_call(func_call)(fn, args))
		return 1;
	if (fn->hooks && hook_apply(parser, &fn->hooks
				->times[HOOK_FUNC_CALL_AFTER]))
		return 1;
	for (int i = 0; i < fn->argc; i++)
		free_yz_val(args[i]);
	free(args);
	return 0;
err_free_args:
	for (int i = 0; i < fn->argc; i++)
		free_yz_val(args[i]);
	free(args);
	return 1;
}

int parse_func_def(struct parser *parser)
{
	struct scope *dest_scope = parser->stat.has_pub
		? parser->scope_pub : parser->scope;
	struct symbol *result = calloc(1, sizeof(*result));
	struct lexer_tok tok;
	struct scope fn_scope = {
		.fn = result,
		.indent = parser->scope->indent,
		.parent = parser->scope,
		.status = backend_call(scope_begin)()
	};
	backend_func_def_handle *handle = NULL;
	parser->scope = &fn_scope;
	if (scope_check_is_correct(&fn_scope))
		goto err_free_result;
	if (func_def_read_name(parser))
		goto err_free_result;
	if (func_def_check_main(result->name.s, result->name.len)) {
		if (func_def_main(parser))
			goto err_free_result;
		return 0;
	}
	if (func_def_read_args(parser))
		goto err_free_result;
	if (func_def_read_type(parser))
		goto err_free_result;
	if (func_def_inherit_decorators(&parser->stat.decorators, result))
		goto err_free_result;
	result->flags.in_block = 1;
	result->parse_function = parse_func_call;
	result->type = SYM_FUNC;
	if (lexer_read_tok(&tok, &parser->lexer))
		goto err_free_result;
	if (symbol_register(result, &dest_scope->sym_groups[SYMG_FUNC]))
		goto err_free_result;
	if (tok.type == TOK_TYPE_NEXT_LINE)
		return func_def_end_scope(parser, NULL);
	if (tok.type != TOK_TYPE_BLOCK_START)
		goto err_free_and_pop_result;
	handle = backend_call(func_def)(result, parser->stat.has_pub, 0);
	if (handle == NULL)
		goto err_free_and_pop_result;
	if (parse_block(parser))
		goto err_free_and_pop_result;
	return func_def_end_scope(parser, handle);
err_free_result:
	free_symbol(result);
	func_def_end_scope(parser, NULL);
	goto err_free_scope_status;
err_free_and_pop_result:
	free_symbol(symbol_pop(&dest_scope->sym_groups[SYMG_FUNC]));
	func_def_end_scope(parser, NULL);
err_free_scope_status:
	backend_call(scope_free)(fn_scope.status);
	return 1;
}

int parse_func_ret(struct parser *parser)
{
	struct expr *expr = NULL;
	yz_val *val = NULL;
	if ((expr = parse_expr(parser, 1)) == NULL)
		return err_print_pos(__func__, "Cannot parse expr!",
				parser->lexer.line, parser->lexer.column);
	if (expr_apply(parser, expr) > 0)
		goto err_cannot_apply_expr;
	if ((val = func_ret_get_val(parser->scope->fn, expr)) == NULL)
		goto err_get_val_failed;
	if (backend_call(func_ret)(val, strncmp(parser->scope->fn->name.s,
					"main", 4) == 0))
		goto err_backend_failed;
	free_yz_val(val);
	return 0;
err_cannot_apply_expr:
	free_expr(expr);
	return err_print_pos(__func__, "Cannot apply expr!",
			parser->lexer.line, parser->lexer.column);
err_get_val_failed:
	return err_print_pos(__func__, "Get value failed!",
			parser->lexer.line, parser->lexer.column);
err_backend_failed:
	free_yz_val(val);
	return err_print_pos(__func__, "Backend call failed!",
			parser->lexer.line, parser->lexer.column);
}
