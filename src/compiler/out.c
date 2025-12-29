/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "cache.h"
#include "out.h"
#include "../debug.h"
#include "../flags.h"
#include "../panic.h"
#include <mcb/mcb.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int assemble(struct mcb_context *mcb, const char *src);
static char *get_file_output_name(const char *src);

int assemble(struct mcb_context *mcb,
		const char *src)
{
	size_t src_len = strlen(src);
	char *output = calloc(src_len + 1, sizeof(char));
	strncpy(output, src, src_len);
	output[src_len - 1] = 'o';
	if (MCB_CALL(mcb, assemble, src, output))
		return 1;
	debug_printf("assembled file %s", output);
	free(output);
	return 0;
}

char *get_file_output_name(const char *src)
{
	size_t result_len = strlen(src) + amc_global_cache.dir.len + 4;
	char *result = calloc(result_len, sizeof(char));
	snprintf(result, result_len, "%s/%s.s", amc_global_cache.dir.s, src);
	return result;
}

int gen_file_output(struct mcb_context *mcb, const char *src_fpath)
{
	FILE *fp = stdout;
	char *output = "\x1b[32mstdout\x1b[0m";

	if (!test_amc_flags(AMC_FLAGS_STDOUT_MODE)) {
		output = get_file_output_name(src_fpath);
		fp = fopen(output, "w");
	}

	debug_printf("generate code to %s", output);
	if (MCB_CALL(mcb, gen_code, fp))
		panic("failed to generate code");

	if (test_amc_flags(AMC_FLAGS_STDOUT_MODE)) {
		debug_print("generated");
		return 0;
	}

	if (!test_amc_flags(AMC_FLAGS_COMPILE_ONLY)) {
		if (assemble(mcb, output))
			panic("failed to assemble");
	}

	fclose(fp);
	free(output);
	return 0;
}
