/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef AMC_PARSER_UTILS_H
#define AMC_PARSER_UTILS_H
#include "../../utils/cint.h"

int err_print_pos(const char *name, const char *msg,
		i64 orig_line, i64 orig_column);

#endif
