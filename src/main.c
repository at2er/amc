/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "flags.h"
#include "module.h"
#include "parser.h"
#include "die.h"
#include <getarg.h>
#include <stdlib.h>

#define AMC_VERSION "0.1"

// static int backend_init(struct mcb_context *context);
static int opt_as(int argc, char *argv[], struct option *opt);
static int opt_as_flags(int argc, char *argv[], struct option *opt);
static int opt_ld(int argc, char *argv[], struct option *opt);
static int opt_ld_flags(int argc, char *argv[], struct option *opt);
static int opt_link(int argc, char *argv[], struct option *opt);
static int opt_output(int argc, char *argv[], struct option *opt);
static int opt_read_src(int argc, char *argv[], struct option *opt);
static int opt_root_mod(int argc, char *argv[], struct option *opt);
static int print_version(void);

struct amc_flags_t amc_flags;

// static enum MCB_MODE backend_mode = MCB_MODE_GNU_ASM;
static const char *output = NULL;
static const char *src_path = NULL;

// {{{ options define
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
// }}}

// {{{ option parsers define
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
	output = argv[0];
	return 0;
}

int opt_read_src(int argc, char *argv[], struct option *opt)
{
	if (argc > 1)
		return 1;
	src_path = argv[0];
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

// }}}

int main(int argc, char *argv[])
{
	// struct mcb_context mcb_con;
	struct parser parser = {0};
	struct yz_module root_mod = {0};
	if (getarg(argc, argv, options))
		return 1;
	if (argc < 2)
		return print_version();
	yz_module_init(&root_mod);
	parser_init(&parser, &root_mod);
	if (parse_file(&parser, src_path))
		return 1;
	return 0;
}
