/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "compiler/cache.h"
#include "compiler/path_max.h"
#include "flags.h"
#include "module.h"
#include "panic.h"
#include "parser.h"
#include <getarg.h>
#include <mcb/mcb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __unix__
#include <unistd.h>
#else
#error unsupport platform
#endif

static void fini_mcb(struct mcb_context *mcb);
static void init_cwd(char **self);
static void init_mcb(struct mcb_context *mcb);
static int parse_cmdline(int argc, char *argv[]);

uint32_t amc_flags = 0;
struct global_cache amc_global_cache = {0};

// static enum MCB_MODE backend_mode = MCB_MODE_GNU_ASM;
static char *cwd = NULL;
static const char *output = "a.out";
static struct parser parser;
static struct yz_module root_mod;
static const char *root_file = NULL;

#include "usage.def"

static struct option options[] = {
	OPT_FLAG("debug",  NO_SHORT_NAME,   &amc_flags, AMC_FLAGS_DEBUG),
	OPT_FLAG("stdout", NO_SHORT_NAME,   &amc_flags, AMC_FLAGS_STDOUT_MODE),
	OPT_HELP("help",       'h',          usages),
	OPT_FLAG("no-cache", NO_SHORT_NAME, &amc_flags, AMC_FLAGS_NO_CACHE),
	OPT_STRING("outdir", NO_SHORT_NAME, &amc_global_cache.dir.s),
	OPT_STRING("output",   'o',          &output),
	OPT_FLAG(NO_LONG_NAME, 'S',         &amc_flags, AMC_FLAGS_COMPILE_ONLY),
	OPT_END
};

void fini_mcb(struct mcb_context *mcb)
{
	FILE *fp;
	if (!test_amc_flags(AMC_FLAGS_STDOUT_MODE)) {
		if (!(fp = fopen(output, "w")))
			panicf("failed to open file '%s'", output);
		if (MCB_CALL(mcb, link, output))
			panic("failed to call mcb.link");
		fclose(fp);
	}
	if (mcb_fini(mcb))
		panic("failed to call mcb_done");
}

void init_cwd(char **self)
{
	char *result = calloc(PATH_MAX + 1, sizeof(char));
	result = getcwd(result, PATH_MAX);
	if (!result)
		panic("failed to get current working directory");
	*self = result;
}

void init_mcb(struct mcb_context *mcb)
{
	if (mcb_init(mcb, MCB_MODE_GNU_ASM))
		panic("failed to init mcb");
}

int parse_cmdline(int argc, char *argv[])
{
	enum GETARG_RESULT ret;

	GETARG_BEGIN(ret, argc, argv, options) {
	case GETARG_RESULT_APPLIED_HELP_OPT:
		return 1;
	case GETARG_RESULT_SUCCESSFUL:
		break;
	case GETARG_RESULT_UNKNOWN:
		if (root_file)
			goto err_had_root_file;
		root_file = *argv;
		GETARG_SHIFT(argc, argv);
		break;
	default: goto err_failed_to_parse;
	} GETARG_END;

	return 0;
err_failed_to_parse:
	panic("failed to parse command line arguments");
	return 1;
err_had_root_file:
	panic("had root file");
	return 1;
}

int main(int argc, char *argv[])
{
	struct mcb_context mcb = {0};
	if (parse_cmdline(argc, argv))
		return 1;
	if (!root_file)
		die("no input\n");
	init_cwd(&cwd);
	init_global_cache(&amc_global_cache, cwd, amc_global_cache.dir.s);
	init_mcb(&mcb);
	init_module(&root_mod);
	init_parser(&parser, &root_mod, &mcb);
	if (parse_file(&parser, root_file))
		return 1;

	fini_mcb(&mcb);
	return 0;
}
