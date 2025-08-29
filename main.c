#include "utils/die.h"

#include "include/backend.h"
#include "include/parser.h"
#include <string.h>

/**
 * Please install 'libgetarg'(https://github.com/at2er/libgetarg)
 */
#include <getarg.h>

#define AMC_VERSION "0.1"

static const char *src = NULL;

static int err_no_input(void);
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

int err_no_input(void)
{
	die("amc: \x1b[31merror\x1b[0m: no input file!\n");
	return 1;
}

int opt_as(int argc, char *argv[], struct option *opt)
{
	backend_assembler = argv[0];
	return 0;
}

int opt_as_flags(int argc, char *argv[], struct option *opt)
{
	return backend_append_assembler_flags(argv[0]);
}

int opt_ld(int argc, char *argv[], struct option *opt)
{
	backend_linker = argv[0];
	return 0;
}

int opt_ld_flags(int argc, char *argv[], struct option *opt)
{
	return backend_append_linker_flags(argv[0]);
}

int opt_link(int argc, char *argv[], struct option *opt)
{
	return backend_append_lib(argv[0]);
}

int opt_output(int argc, char *argv[], struct option *opt)
{
	global_parser.output.s = argv[0];
	global_parser.output.len = strlen(argv[0]);
	return 0;
}

int opt_read_src(int argc, char *argv[], struct option *opt)
{
	src = argv[0];
	return 0;
}

int opt_root_mod(int argc, char *argv[], struct option *opt)
{
	global_parser.root_mod.s = argv[0];
	global_parser.root_mod.len = strlen(global_parser.root_mod.s);
	return 0;
}

int print_version(void)
{
	die("Atom compiler(Yuan Zi Compiler) v%s\n", AMC_VERSION);
	return 0;
}

int main(int argc, char *argv[])
{
	if (getarg(argc, argv, options))
		return 1;
	if (backend_init(argc, argv))
		die("amc: backend_init: cannot init backend.");
	if (argc < 2)
		return print_version();
	if (src == NULL)
		return err_no_input();
	if (parser_init(src))
		return 1;
	return 0;
}
