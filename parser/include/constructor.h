/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef AMC_PARSER_CONSTRUCTOR_H
#define AMC_PARSER_CONSTRUCTOR_H
#include "../../include/symbol.h"
#include "../../include/val.h"

struct constructor_handle {
	int index;
	int len;
	struct parser *parser;
	struct symbol *sym;
	yz_val **vs;
};

void free_constructor_handle(struct constructor_handle *self);
void free_constructor_handle_no_self(struct constructor_handle *self);

#endif
