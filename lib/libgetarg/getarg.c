#include "getarg.h"
#include <limits.h>
#include <sctrie.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK_OPTS_END(OPT) ((OPT).long_name == NULL\
		&& (OPT).short_name == '\0')

struct long_opt_node;
struct long_opt_node {
	struct long_opt_node *children[UCHAR_MAX + 1];
	struct option *opt;
};

static getarg_opt_parser default_arg_parser = NULL;
static struct option *short_opts[UCHAR_MAX + 1];
static struct long_opt_node *long_opts[UCHAR_MAX + 1];

static int apply_list_arg(int argc, char *argv[], struct option *opt);
static int apply_opt(int argc, char *argv[], struct option *opt,
		struct option *opts);
static int apply_opt_queue(int argc, char *argv[], int queue_len,
		struct option **queue);
static int apply_single_arg(int argc, char *argv[], struct option *opt);
static void help_opt_elem(struct option *opt, unsigned int max_opt_len);
static int init(struct option *opts);
static int init_long_opt(struct option *opt);
static struct option *long_opt_find(char *name);
static int parse_long_opt(int argc, char *argv[], struct option *opts);
static int parse_opt(int argc, char *argv[], struct option *opts);
static int parse_short_opt(int argc, char *argv[], struct option *opts);

int apply_list_arg(int argc, char *argv[], struct option *opt)
{
	int len = 0;
	for (len = 0; len < argc; len++) {
		if (argv[len][0] == '-')
			break;
	}
	if (len == 0)
		return apply_single_arg(0, NULL, opt);
	if (opt->parse(len, argv, opt))
		return -1;
	return len;
}

int apply_opt(int argc, char *argv[], struct option *opt, struct option *opts)
{
	if (opt == NULL)
		goto err_unknown_opt;
	switch (opt->type) {
	case GETARG_HELP_OPT:
		getarg_help_opt(opt, opts);
		return 0;
		break;
	case GETARG_LIST_ARG:
		return apply_list_arg(argc - 1, &argv[1], opt);
		break;
	case GETARG_NO_ARG:
		if (opt->parse(0, NULL, opt))
			return -1;
		return 0;
		break;
	case GETARG_SINGLE_ARG:
		return apply_single_arg(argc - 1, &argv[1], opt);
		break;
	}
	if (argv[1][0] == '-') {
		if (!opt->is_optional_arg)
			return -1;
		if (opt->parse(0, NULL, opt))
			return -1;
		return 0;
	}
	if (opt->parse(1, &argv[1], opt))
		return -1;
	return 0;
err_unknown_opt:
	fprintf(stderr, "libgetarg: Unknown option\n");
	return -1;
}

int apply_opt_queue(int argc, char *argv[], int queue_len,
		struct option **queue)
{
	for (int i = 0; i < queue_len; i++) {
		if (queue[i]->parse(0, NULL, queue[i]))
			return -1;
	}
	return 0;
}

int apply_single_arg(int argc, char *argv[], struct option *opt)
{
	if (argc <= 0) {
		if (!opt->is_optional_arg)
			goto err_arg_not_found;
		if (opt->parse(0, NULL, opt))
			return -1;
		return 0;
	}
	if (opt->parse(1, argv, opt))
		return -1;
	return 1;
err_arg_not_found:
	fprintf(stderr, "libgetarg: Arguments for option '%s:%c' not found!\n",
			opt->long_name, opt->short_name);
	return -1;
}

void help_opt_elem(struct option *opt, unsigned int max_opt_len)
{
	if (opt->short_name == '\0') {
		printf("      --%s:  %*s%s\n",
				opt->long_name,
				(int)(max_opt_len - strlen(opt->long_name)),
				"",
				opt->document);
	} else {
		printf("  -%c, --%s:  %*s%s\n",
				opt->short_name,
				opt->long_name,
				(int)(max_opt_len - strlen(opt->long_name)),
				"",
				opt->document);
	}
	if (opt->opt_doc != NULL)
		printf("    %*s%s\n", (int)(9 + max_opt_len),
				"", opt->opt_doc);
}

int init(struct option *opts)
{
	int i = 0;
	for (; !CHECK_OPTS_END(opts[i]); i++) {
		if (opts[i].long_name != NULL)
			if (init_long_opt(&opts[i]))
				return 1;
		if (opts[i].short_name == '\0')
			continue;
		short_opts[(uint8_t)opts[i].short_name] = &opts[i];
	}
	default_arg_parser = opts[i].parse;
	return 0;
}

int init_long_opt(struct option *opt)
{
	struct long_opt_node *node =
		sctrie_append_elem(long_opts, sizeof(*node),
				opt->long_name, strlen(opt->long_name));
	if (node == NULL)
		return 1;
	node->opt = opt;
	return 0;
}

struct option *long_opt_find(char *name)
{
	struct long_opt_node *node =
		sctrie_find_elem(long_opts, &name[2], strlen(name) - 2);
	if (node == NULL)
		return NULL;
	return node->opt;
}

int parse_long_opt(int argc, char *argv[], struct option *opts)
{
	return apply_opt(argc, argv, long_opt_find(argv[0]), opts);
}

int parse_opt(int argc, char *argv[], struct option *opts)
{
	if (argv[0][1] == '-')
		return parse_long_opt(argc, argv, opts);
	return parse_short_opt(argc, argv, opts);
}

int parse_short_opt(int argc, char *argv[], struct option *opts)
{
	int apply_queue_len = 0;
	struct option **apply_queue = NULL, *tmp = NULL;
	if (argv[0][2] == '\0')
		return apply_opt(argc, argv, short_opts[(uint8_t)argv[0][1]],
				opts);
	apply_queue_len = strlen(argv[0]) - 1;
	apply_queue = malloc(apply_queue_len * sizeof(*apply_queue));
	for (int i = 1; argv[0][i] != '\0'; i++) {
		if ((tmp = short_opts[(uint8_t)argv[0][i]]) == NULL)
			goto err_unknown_opt;
		if (!tmp->is_optional_arg && tmp->type == GETARG_LIST_ARG)
			goto err_multiple_short_opt_list;
		apply_queue[i - 1] = tmp;
	}
	if (apply_opt_queue(argc, argv, apply_queue_len, apply_queue))
		goto err_apply_failed;
	free(apply_queue);
	return 0;
err_unknown_opt:
	free(apply_queue);
	fprintf(stderr, "libgetarg: Unknown short option: \"%s\"\n", argv[0]);
	return -1;
err_multiple_short_opt_list:
	free(apply_queue);
	fprintf(stderr, "libgetarg: Multiple short option,"
			" but need list arguments: \"%s\"\n",
			argv[0]);
	return -1;
err_apply_failed:
	free(apply_queue);
	fprintf(stderr, "libgetarg: Apply short option failed: \"%s\"\n",
			argv[0]);
	return -1;
}

int getarg(int argc, char *argv[], struct option *opts)
{
	int default_argc = 0;
	char **default_argv = NULL;
	int i, jump;
	if (init(opts))
		goto err_init_failed;
	for (i = 1, jump = 0; i < argc; i++) {
		if (argv[i][0] == '-') {
			if ((jump = parse_opt(argc - i, &argv[i], opts)) < 0)
				goto err_free_default_argv;
			i += jump;
			if (i > INT_MAX)
				goto err_free_default_argv;
			jump = 0;
			continue;
		}
		default_argc++;
		default_argv = realloc(default_argv,
				default_argc * sizeof(char*));
		default_argv[default_argc - 1] = argv[i];
	}
	sctrie_free_tree_noself(long_opts, free);
	if (default_argv == NULL)
		return 0;
	if (default_arg_parser == NULL)
		goto err_default_arg_parser_not_exist;
	if (default_arg_parser(default_argc, default_argv, opts))
		goto err_free_default_argv;
	free(default_argv);
	return 0;
err_init_failed:
	fprintf(stderr, "libgetarg: Init failed!\n");
	return 1;
err_default_arg_parser_not_exist:
	fprintf(stderr, "libgetarg: "
			"Default arguments parser doesn't exist!\n");
	return 1;
err_free_default_argv:
	if (default_argv == NULL)
		return 1;
	free(default_argv);
	return 1;
}

void getarg_help_opt(struct option *opt, struct option *opts)
{
	unsigned int max_opt_len = 0;
	printf("Usage: [OPTIONS] ");
	if (opt != NULL && opt->parse != NULL)
		opt->parse(GETARG_HELP_AFTER_USAGE, NULL, opt);
	printf("\n\nOPTIONS:\n");
	for (int i = 0, opt_len; !CHECK_OPTS_END(opts[i]); i++) {
		opt_len = strlen(opts[i].long_name);
		if (max_opt_len < opt_len)
			max_opt_len = opt_len;
	}
	for (int i = 0; !CHECK_OPTS_END(opts[i]); i++)
		help_opt_elem(&opts[i], max_opt_len);
	printf("\n");
	if (opt != NULL && opt->parse != NULL)
		opt->parse(GETARG_HELP_ENDING, NULL, opt);
	exit(0);
}
