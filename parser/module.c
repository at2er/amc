/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "include/keywords.h"
#include "include/lexer.h"
#include "../include/module.h"
#include "../include/parser.h"
#include <stdio.h>
#include <sys/stat.h>

static int import_check_file_exists(str *real_path);
static struct scope *import_parse_file(str *path, struct parser *parent);
static int import_read_path(str *result, struct sclexer *lexer);
static int import_read_path_file(str *result, struct sclexer *lexer);
static yz_module *module_parse_import(struct parser *parser);

int import_check_file_exists(str *real_path)
{
	struct stat st;
	if (stat(real_path->s, &st))
		goto err_no_such_path;
	if (S_ISDIR(st.st_mode))
		goto err_is_dir;
	return 1;
err_is_dir:
	printf("amc: import_check_file_exists: Path: '%s' is directory\n",
			real_path->s);
	return 0;
err_no_such_path:
	printf("amc: import_check_file_exists: No such file: '%s'\n",
			real_path->s);
	return 0;
}

struct scope *import_parse_file(str *path, struct parser *parent)
{
	struct parser *parser = NULL;
	str *real_path = NULL;
	struct scope *result = NULL;
	if ((result = parser_parsed_file_find(path)) != NULL)
		return result;
	if ((real_path = module_path2real(path)) == NULL)
		return NULL;
	if (!import_check_file_exists(real_path))
		goto err_free_real_path;
	if ((parser = parse_file(path, real_path->s)) == NULL)
		goto err_free_real_path;
	result = parser->scope_pub;
	parser->scope_pub = NULL;
	free_parser(parser);
	return result;
err_free_real_path:
	str_free(real_path);
	return NULL;
}

int import_read_path(str *result, struct sclexer *lexer)
{
	int ret = 0;
	while ((ret = import_read_path_file(result, lexer))
			!= LEXER_RESULT_END) {
		if (ret == LEXER_RESULT_FAULT)
			return 1;
	}
	str_append(result, 1, "\0");
	result->len -= 1;
	return 0;
}

int import_read_path_file(str *result, struct sclexer *lexer)
{
	struct lexer_tok tok;
	if (lexer_read_tok(&tok, lexer) || tok.type != TOK_TYPE_STR)
		return 1;
	str_append(result, tok.data.s.len, tok.data.s.s);
	str_append(result, 1, "/");
	return 0;
}

yz_module *module_parse_import(struct parser *parser)
{
	yz_module *err_data = NULL;
	str path = STR_EMPTY;
	yz_module *result = NULL;
	struct scope *scope = NULL;
	if (lexer_try_read_str(&parser->lexer, "import", 6)
			!= TRY_RESULT_HANDLED)
		goto err_miss_import;
	if (import_read_path(&path, &parser->lexer))
		goto err_free_path;
	err_data = parser_imported_find(&parser->imported, &path);
	if (err_data != NULL)
		goto err_imported;
	if ((scope = import_parse_file(&path, parser)) == NULL)
		return NULL;
	result = calloc(1, sizeof(*result));
	result->path.s = path.s;
	result->path.len = path.len;
	result->scope = scope;
	return result;
err_miss_import:
	printf(LEXER_ERR_FMT ERROR_STR"Missing keyword: 'import'.\n",
			LEXER_ERR_FMT_ARG(parser->lexer));
	return NULL;
err_free_path:
	str_free_noself(&path);
	return NULL;
err_imported:
	printf(LEXER_ERR_FMT ERROR_STR"Module: '%s' is "
			"imported by '%s'\n",
			LEXER_ERR_FMT_ARG(parser->lexer),
			path.s, err_data->name.s);
	free(path.s);
	return NULL;
}

int parse_mod(struct parser *parser)
{
	str name = STR_EMPTY;
	yz_module *result = NULL;
	struct lexer_tok tok;
	if (lexer_read_tok(&tok, &parser->lexer) || tok.type != TOK_TYPE_STR)
		return 1;
	str_copy(&tok.data.s, &name);
	if ((result = module_parse_import(parser)) == NULL)
		return 1;
	result->name.s = name.s;
	result->name.len = name.len;
	if (parser_imported_append(&parser->imported, result))
		goto err_free_result;
	return 0;
err_free_result:
	free_yz_module(result);
	return 1;
}

int parse_pub(struct parser *parser)
{
	struct symbol *kw = NULL;
	str token = STR_EMPTY;
	parser->stat.has_pub = 1;
	if (!keyword_find(&token, &kw))
		return 1;
	if (kw->parse_function == parse_func_def
			|| kw->parse_function == parse_struct) {
		parser->sym = kw;
		return kw->parse_function(parser);
	}
	printf(LEXER_ERR_FMT ERROR_STR":\n"
			"| Keyword 'pub' only can use for 'fn' and 'struct'\n",
			LEXER_ERR_FMT_ARG(parser->lexer));
	return 1;
}
