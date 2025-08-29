/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "include/decorator.h"
#include "include/lexer.h"
#include "../include/backend.h"
#include "../include/comptime/hook.h"
#include "../include/parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int dec_c_fn(struct parser *parser, struct hook_callee *callee);
static int dec_syscall(struct parser *parser, struct hook_callee *callee);

static struct decorator decorators[] = {
	{ "c.fn", dec_c_fn, HOOK_FUNC_CALL_AFTER },
	{ "syscall", dec_syscall, HOOK_FUNC_CALL_AFTER }
};

static int decorator_arg_get_val(struct lexer_tok *tok, yz_val *arg);
static int decorator_arg_get_val_sclexer(struct sclexer_tok *tok, yz_val *arg);
static enum LEXER_RESULT parse_decorator_arg(struct sclexer *lexer,
		struct hook_callee *callee);
static int parse_decorator_args(struct sclexer *lexer,
		struct hook_callee *callee);

int dec_c_fn(struct parser *parser, struct hook_callee *callee)
{
	if (callee->argc != 1)
		goto err_arg_failed;
	if (backend_call(dec_c_fn)(&parser->sym->name, parser->sym->argc))
		goto err_backend_failed;
	return 0;
err_arg_failed:
	printf("amc: dec_c_fn: Argument failed\n");
	return 1;
err_backend_failed:
	printf("amc: dec_c_fn: Backend call failed!\n");
	return 1;
}

int dec_syscall(struct parser *parser, struct hook_callee *callee)
{
	if (callee->argc != 1)
		goto err_arg_failed;
	if (!YZ_IS_DIGIT(callee->args[0]->type.type))
		goto err_arg_failed;
	if (backend_call(dec_syscall)(callee->args[0]->data.i,
				parser->sym->argc))
		goto err_backend_failed;
	return 0;
err_arg_failed:
	printf("amc: dec_syscall: Argument failed\n");
	return 1;
err_backend_failed:
	printf("amc: dec_syscall: Backend call failed!\n");
	return 1;
}

int decorator_arg_get_val(struct lexer_tok *tok, yz_val *arg)
{
	switch (tok->type) {
	case TOK_TYPE_SCLEXER:
		return decorator_arg_get_val_sclexer(&tok->self, arg);
	default: break;
	}
	return 1;
}

int decorator_arg_get_val_sclexer(struct sclexer_tok *tok, yz_val *arg)
{
	switch (tok->type) {
	case SCLEXER_TOK_TYPE_INT:
	case SCLEXER_TOK_TYPE_INT_NEG:
		arg->type.type = YZ_I64;
		arg->data.l = tok->type_data.sint;
		return 0;
	default: break;
	}
	return 1;
}

enum LEXER_RESULT
parse_decorator_arg(struct sclexer *lexer, struct hook_callee *callee)
{
	struct lexer_tok tok;
	if (lexer_read_tok(&tok, lexer))
		return LEXER_RESULT_FAULT;
	callee->argc += 1;
	callee->args = realloc(callee->args, sizeof(*callee->args)
			* callee->argc);
	callee->args[callee->argc - 1] = malloc(sizeof(**callee->args));
	if (tok.type == TOK_TYPE_BRACKET_R)
		return LEXER_RESULT_END;
	if (decorator_arg_get_val(&tok, callee->args[callee->argc - 1]))
		return LEXER_RESULT_FAULT;
	return LEXER_RESULT_CONTINUE;
}

int parse_decorator_args(struct sclexer *lexer, struct hook_callee *callee)
{
	enum LEXER_RESULT ret;
	struct lexer_tok tok;
	if (lexer_read_tok(&tok, lexer))
		return 1;
	if (tok.type == TOK_TYPE_NEXT_LINE)
		return 0;
	if (tok.type != TOK_TYPE_BRACKET_L)
		return 1;
	while ((ret = parse_decorator_arg(lexer, callee))
			!= LEXER_RESULT_END) {
		if (ret == LEXER_RESULT_FAULT)
			return 1;
	}
	return 0;
}

struct decorator *get_decorator(str *token)
{
	if (token->s[0] == '@') {
		token->len -= 1;
		token->s = &token->s[1];
	}
	for (int i = 0; i < LENGTH(decorators); i++) {
		if (token->len != strlen(decorators[i].name))
			continue;
		if (strncmp(token->s, decorators[i].name, token->len) == 0)
			return &decorators[i];
	}
	return NULL;
}

int parse_decorator(struct decorators *self, struct sclexer *lexer)
{
	struct hook_callee *callee = NULL;
	struct decorator *dec = NULL;
	char *err_msg;
	str name = STR_EMPTY;
	struct lexer_tok tok;
	if (lexer_read_tok(&tok, lexer))
		return 1;
	if (tok.type != TOK_TYPE_STR)
		return 1;
	str_copy(&tok.data.s, &name);
	if ((dec = get_decorator(&name)) == NULL)
		goto err_dec_not_found;
	callee = calloc(1, sizeof(*callee));
	callee->apply = dec->apply;
	if (parse_decorator_args(lexer, callee))
		return 1;
	if (self->hooks == NULL)
		self->hooks = calloc(1, sizeof(*self->hooks));
	if (hook_append(&self->hooks->times[dec->time], callee))
		return 1;
	self->has = 1;
	return 0;
err_dec_not_found:
	err_msg = str2chr(name.s, name.len);
	printf(LEXER_ERR_FMT ERROR_STR":\n"
			"| Decorator: '%s' not found!\n",
			LEXER_ERR_FMT_ARG_REF(lexer),
			err_msg);
	free(err_msg);
	return 1;
}

void free_decorators_noself(struct decorators *self)
{
	if (self == NULL)
		return;
	free_hooks_noself(self->hooks);
}
