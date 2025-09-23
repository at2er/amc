/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "parser.h"
#include "utils/die.h"
#include <getarg.h>
#include <string.h>

#define AMC_VERSION "0.1"

static int opt_as(int argc, char *argv[], struct option *opt);
static int opt_as_flags(int argc, char *argv[], struct option *opt);
static int opt_ld(int argc, char *argv[], struct option *opt);
static int opt_ld_flags(int argc, char *argv[], struct option *opt);
static int opt_link(int argc, char *argv[], struct option *opt);
static int opt_output(int argc, char *argv[], struct option *opt);
static int opt_read_src(int argc, char *argv[], struct option *opt);
static int opt_root_mod(int argc, char *argv[], struct option *opt);
static int print_version(void);

static struct option options[] = {
	{
		"as", '\0',
		GETARG_SINGLE_ARG, 0,
		opt_as,
		"select assembler",
		NULL
	},
	{
		"as-flags", '\0',
		GETARG_SINGLE_ARG, 0,
		opt_as_flags,
		"assembler options",
		"Use '-f elf64 -p gas' and set assembler"
			" to 'yasm' for assembler"
	},
	{
		"help", 'h',
		GETARG_HELP_OPT, 0,
		NULL,
		"show help documents",
		NULL
	},
	{
		"ld", '\0',
		GETARG_SINGLE_ARG, 0,
		opt_ld,
		"select linker",
		"Use 'ar' to generate a static library"
	},
	{
		"ld-flags", '\0',
		GETARG_SINGLE_ARG, 0,
		opt_ld_flags,
		"linker options",
		"Use 'rcs' and set linker to 'ar' for linker"
			" to generate a static library"
	},
	{
		"link", 'l',
		GETARG_SINGLE_ARG, 0,
		opt_link,
		"link other librarys",
		"Don't use this option to link yz lib!"
	},
	{
		"output", 'o',
		GETARG_SINGLE_ARG, 0,
		opt_output,
		"output file name",
		NULL
	},
	{
		"root-mod", '\0',
		GETARG_SINGLE_ARG, 0,
		opt_root_mod,
		"set root module name",
		NULL
	},
	{
		NULL, '\0',
		GETARG_LIST_ARG, 0,
		opt_read_src,
		"source file",
		NULL
	}
};

int opt_as(int argc, char *argv[], struct option *opt)
{
	return 0;
}

int opt_as_flags(int argc, char *argv[], struct option *opt)
{
	return 0;
}

int opt_ld(int argc, char *argv[], struct option *opt)
{
	return 0;
}

int opt_ld_flags(int argc, char *argv[], struct option *opt)
{
	return 0;
}

int opt_link(int argc, char *argv[], struct option *opt)
{
	return 0;
}

int opt_output(int argc, char *argv[], struct option *opt)
{
	return 0;
}

int opt_read_src(int argc, char *argv[], struct option *opt)
{
	return 0;
}

int opt_root_mod(int argc, char *argv[], struct option *opt)
{
	return 0;
}

int print_version(void)
{
	die("Atom compiler(Yuan Zi Compiler) v%s\n", AMC_VERSION);
	return 0;
}

int main(int argc, char *argv[])
{
	struct parser parser;
	if (getarg(argc, argv, options))
		return 1;
	if (argc < 2)
		return print_version();

	lexer_init(&parser.lexer);
	if (lexer_parse_file(&parser.lexer, argv[1]))
		return 1;
	lexer_print_block(&parser.lexer.root);

	return 0;
}
