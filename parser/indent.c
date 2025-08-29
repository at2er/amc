/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "include/indent.h"

static int cond(struct sclexer_str_slice *result, struct sclexer *lexer);

int cond(struct sclexer_str_slice *result, struct sclexer *lexer)
{
	if (lexer->cur[0] == '\t')
		return 1;
	return 0;
}

int indent_read(struct sclexer *lexer)
{
	struct sclexer_str_slice slice;
	sclexer_read_while(&slice, lexer, cond);
	return slice.len;
}
