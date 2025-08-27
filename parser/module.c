/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "include/keywords.h"
#include "include/utils.h"
#include "../include/module.h"
#include "../include/parser.h"
#include "../include/token.h"
#include <stdio.h>
#include <sys/stat.h>

static int import_check_file_exists(str *real_path);
static struct scope *import_parse_file(str *path, struct parser *parent);
static int import_read_path(str *result, struct file *file);
static int import_read_path_check_end(struct file *f);
static int import_read_path_dir(str *result, struct file *f);
static int import_read_path_file(str *result, struct file *f);
static int module_check_is_import(struct file *f);
static yz_module *module_parse_import(struct parser *parser);
static int module_read_name(str *result, struct file *f);

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
	struct file f;
	struct parser *parser = NULL;
	str *real_path = NULL;
	struct scope *result = NULL;
	if ((result = parser_parsed_file_find(path)) != NULL)
		return result;
	if ((real_path = module_path2real(path)) == NULL)
		return NULL;
	if (!import_check_file_exists(real_path))
		goto err_free_real_path;
	if ((parser = parse_file(path, real_path->s, &f)) == NULL)
		goto err_free_real_path;
	result = parser->scope_pub;
	parser->scope_pub = NULL;
	free_file_noself(&f);
	free_parser(parser);
	return result;
err_free_real_path:
	str_free(real_path);
	return NULL;
}

int import_read_path(str *result, struct file *f)
{
	int ret = 0;
	while ((ret = import_read_path_file(result, f)) != -1) {
		if (ret > 0)
			return 1;
	}
	str_append(result, 1, "\0");
	result->len -= 1;
	return 0;
}

int import_read_path_check_end(struct file *f)
{
	i64 orig_column = f->cur_column,
			orig_line = f->cur_line,
			orig_pos = f->pos;
	if (f->src[f->pos] == '.')
		return 0;
	if (!parse_comment(f) && f->src[f->pos] != '\n')
		return 2;
	if (f->src[f->pos] == '\n')
		file_line_next(f);
	file_skip_space(f);
	if (f->src[f->pos] == '.')
		return 0;
	f->cur_column = orig_column;
	f->cur_line = orig_line;
	f->pos = orig_pos;
	return 1;
}

int import_read_path_dir(str *result, struct file *f)
{
	if (f->src[f->pos] != '.')
		return 1;
	file_pos_next(f);
	if (!file_try_skip_space(f))
		return 1;
	str_append(result, 1, "/");
	return 0;
}

int import_read_path_file(str *result, struct file *f)
{
	str token = TOKEN_NEW;
	if (token_read_before(" \t\n;.", &token, f) == NULL)
		return 1;
	file_skip_space(f);
	if (!check_module_name(&token))
		return 1;
	str_append(result, token.len, token.s);
	if (import_read_path_check_end(f))
		return -1;
	return import_read_path_dir(result, f);
}

int module_check_is_import(struct file *f)
{
	str expect = {.len = 6, .s = "import"};
	return token_try_read(&expect, f);
}

yz_module *module_parse_import(struct parser *parser)
{
	yz_module *err_data = NULL;
	str path = STR_EMPTY;
	yz_module *result = NULL;
	struct scope *scope = NULL;
	if (!module_check_is_import(parser->f))
		goto err_miss_import;
	if (import_read_path(&path, parser->f))
		return NULL;
	if ((err_data = parser_imported_find(&parser->imported, &path)) != NULL)
		goto err_imported;
	if ((scope = import_parse_file(&path, parser)) == NULL)
		return NULL;
	result = calloc(1, sizeof(*result));
	result->path.s = path.s;
	result->path.len = path.len;
	result->scope = scope;
	return result;
err_imported:
	printf("amc: module_parse_import: %lld,%lld: Module: '%s' is "
			"imported by '%s'\n",
			parser->f->cur_line, parser->f->cur_column, path.s, err_data->name.s);
	free(path.s);
	return NULL;
err_miss_import:
	printf("amc: module_parse_import: %lld,%lld: Missing keyword: 'import'.\n",
			parser->f->cur_line, parser->f->cur_column);
	return NULL;
}

int module_read_name(str *result, struct file *f)
{
	i64 orig_column = f->cur_column,
	    orig_line = f->cur_line;
	str token = TOKEN_NEW;
	if (module_check_is_import(f))
		goto err_miss_name;
	if (token_next(&token, f))
		return 1;
	str_copy(&token, result);
	if (!check_module_name(result))
		return err_print_pos(__func__, NULL,
				orig_line, orig_column);
	return 0;
err_miss_name:
	printf("amc: module_read_name: %lld,%lld: Missing module name!\n",
			orig_line, orig_column);
	return 1;
}

int parse_mod(struct parser *parser)
{
	str name = TOKEN_NEW;
	yz_module *result = NULL;
	if (module_read_name(&name, parser->f))
		return 1;
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
	str token = TOKEN_NEW;
	parser->stat.has_pub = 1;
	keyword_end(parser->f);
	if (token_next(&token, parser->f))
		return 1;
	if (!keyword_find(&token, &kw))
		return 1;
	if (kw->parse_function == parse_func_def
			|| kw->parse_function == parse_struct) {
		parser->sym = kw;
		return kw->parse_function(parser);
	}
	printf("amc: parse_pub: %lld,%lld: error:\n"
			"| Keyword 'pub' only can use for 'fn' and 'struct'\n",
			parser->f->cur_line, parser->f->cur_column);
	return 1;
}
