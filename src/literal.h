/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef AMC_LITERAL_H
#define AMC_LITERAL_H
#include "type.h"
#include <stdbool.h>
#include <stdint.h>

union yz_literal_data {
	struct yz_expr *expr;
	struct yz_func_call *func_call;
	struct yz_ident *ident;
	uint64_t uint;
	int64_t sint;
};

struct yz_literal {
	union yz_literal_data data;
	struct yz_type type;
};

void free_yz_literal(struct yz_literal *self);

enum YZ_TYPE get_integer_literal_type(bool is_signed, int64_t src);

void print_yz_literal(const struct yz_literal *self, uint16_t depth);

#endif
