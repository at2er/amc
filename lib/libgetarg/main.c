/* test */
#include <stdio.h>
#include <stdlib.h>
#include "getarg.h"

#define LENGTH(X) (sizeof((X)) / sizeof((X)[0]))

static int test_default_arg(int argc, char **argv, struct option *opt)
{
	printf("libgetarg: Default arguments: %d:\n", argc);
	for (int i = 0; i < argc; i++)
		printf("| %s\n", argv[i]);
	return 0;
}

static int test_help_ext(int argc, char **argv, struct option *opt)
{
	if (argc == 0) {
		printf("INPUT");
		return 0;
	}
	printf("INPUT:\n"
	       "\tSome file\n");
	return 0;
}

static int test_list(int argc, char **argv, struct option *opt)
{
	printf("libgetarg: %d:\n", argc);
	for (int i = 0; i < argc; i++)
		printf("| %s\n", argv[i]);
	return 0;
}

static int test_no_arg(int argc, char **argv, struct option *opt)
{
	printf("libgetarg: \"%c:%s\"\n", opt->short_name, opt->long_name);
	return 0;
}

static int test_root_mod(int argc, char **argv, struct option *opt)
{
	printf("libgetarg: \"%c:%s\"\n", opt->short_name, opt->long_name);
	return 0;
}

int main(int argc, char *argv[])
{
	struct option opts[] = {
		{
			"help", 'h',
			GETARG_HELP_OPT, 0,
			test_help_ext,
			"show help documents",
			NULL,
		},
		{
			"list",  'l',
			GETARG_LIST_ARG, 0,
			test_list,
			"test list arg",
			"Option document test"
		},
		{
			"no-arg", 'n',
			GETARG_NO_ARG, 0,
			test_no_arg,
			"test no arg",
			NULL
		},
		{
			"root-mod", '\0',
			GETARG_LIST_ARG, 0,
			test_root_mod,
			"set root module name",
			NULL
		},
		{
			NULL, '\0',
			GETARG_LIST_ARG, 0,
			test_default_arg,
			"test default arg",
			NULL
		}
	};

	return getarg(argc, argv, opts);
}
