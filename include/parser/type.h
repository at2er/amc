/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef AMC_PARSER_TYPE_H
#define AMC_PARSER_TYPE_H
#include "parser.h"
#include "../type.h"
#include "../../utils/str/str.h"

int parser_get_ident_with_type(struct parser *parser,
		str *ident,
		struct yz_type *type);

#endif
