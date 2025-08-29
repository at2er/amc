/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef AMC_PARSER_IDENTIFIER_H
#define AMC_PARSER_IDENTIFIER_H
#include "../../include/op.h"
#include "../../include/symbol.h"
#include "../../include/val.h"
#include <sclexer.h>

int identifier_assign_get_val(struct parser *parser, yz_type *dest_type,
		yz_val **result);
int identifier_assign_val(struct parser *parser, struct symbol *sym,
		enum OP_ID mode);
int identifier_check_can_assign_val(struct parser *parser,
		struct symbol *ident, yz_val *val);
int identifier_check_mut(struct sclexer *lexer);
yz_val *identifier_handle_expr_val(struct expr *e, yz_type *type);
int identifier_handle_val_type(yz_type *src, yz_type *dest);

#endif
