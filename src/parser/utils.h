/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef AMC_PARSER_UTILS_H
#define AMC_PARSER_UTILS_H
#include "../parser.h"
#include "../str.h"
#include "../type.h"

int get_ident_with_type(struct parser *parser,
		str *ident,
		struct yz_type *type);

#endif
