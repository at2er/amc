/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "include/utils.h"
#include "../include/backend.h"
#include <stdio.h>

int err_print_pos(const char *name, const char *msg,
		i64 orig_line, i64 orig_column)
{
	if (msg != NULL) {
		printf("| %s: %lu,%lu: %s\n",
				name, orig_line, orig_column, msg);
	} else {
		printf("| %s: %lu,%lu\n", name, orig_line, orig_column);
	}
	backend_stop(BE_STOP_SIGNAL_ERR);
	return 1;
}
