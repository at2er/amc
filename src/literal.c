/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "die.h"
#include "expr.h"
#include "fn.h"
#include "literal.h"
#include "panic.h"
#include "type.h"
#include <stdbool.h>
#include <stdint.h>

static enum YZ_TYPE get_negtive_interge_type(int64_t src);
static enum YZ_TYPE get_signed_integer_type(int64_t src);
static enum YZ_TYPE get_unsigned_integer_type(uint64_t src);

enum YZ_TYPE get_negtive_interge_type(int64_t src)
{
	if (src >= -INT8_MAX)  return YZ_I8_LITERAL;
	if (src >= -INT16_MAX) return YZ_I16_LITERAL;
	if (src >= -INT32_MAX) return YZ_I32_LITERAL;
	if (src >= -INT64_MAX) return YZ_I64_LITERAL;
	return -1;
}

enum YZ_TYPE get_signed_integer_type(int64_t src)
{
	if (src < 0)
		return get_negtive_interge_type(src);
	if (src <= INT8_MAX)  return YZ_I8_LITERAL;
	if (src <= INT16_MAX) return YZ_I16_LITERAL;
	if (src <= INT32_MAX) return YZ_I32_LITERAL;
	if (src <= INT64_MAX) return YZ_I64_LITERAL;
	return -1;
}

enum YZ_TYPE get_unsigned_integer_type(uint64_t src)
{
	if (src <= UINT8_MAX)  return YZ_U8_LITERAL;
	if (src <= UINT16_MAX) return YZ_U16_LITERAL;
	if (src <= UINT32_MAX) return YZ_U32_LITERAL;
	if (src <= UINT64_MAX) return YZ_U64_LITERAL;
	return -1;
}

void free_yz_literal(struct yz_literal *self)
{
	if (!self)
		return;
	switch (self->type.type) {
	case YZ_EXPR:      free_yz_expr(self->data.expr);           break;
	case YZ_FUNC_CALL: free_yz_func_call(self->data.func_call); break;
	default:
		panicf("unprocessed type '%s'",
				type_get_str(self->type.type));
		break;
	}
	free_yz_type_noself(&self->type);
}

enum YZ_TYPE get_integer_literal_type(bool is_signed, int64_t src)
{
	if (is_signed)
		return get_signed_integer_type(src);
	return get_unsigned_integer_type(src);
}

void print_yz_literal(const struct yz_literal *self, uint16_t depth)
{
	switch (self->type.type) {
	case YZ_I8_LITERAL: case YZ_I16_LITERAL:
	case YZ_I32_LITERAL: case YZ_I64_LITERAL:
		printf("%*sterm: <type: '%s'> <value: '%ld'>\n",
				depth, "",
				type_get_str(self->type.type),
				self->data.sint);
		break;
	case YZ_U8_LITERAL: case YZ_U16_LITERAL:
	case YZ_U32_LITERAL: case YZ_U64_LITERAL:
		printf("%*sterm: <type: '%s'> <value: '%lu'>\n",
				depth, "",
				type_get_str(self->type.type),
				self->data.uint);
		break;
	case YZ_EXPR:
		print_yz_expr(self->data.expr, depth);
		break;
	case YZ_FUNC_CALL:
		print_yz_func_call(self->data.func_call, depth);
		break;
	default:
		printf("%*sterm: <type: '%s'>\n",
				depth, "",
				type_get_str(self->type.type));
		break;
	}
}
