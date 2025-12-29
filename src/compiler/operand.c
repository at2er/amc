/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "operand.h"
#include "../die.h"
#include "../panic.h"
#include "../type.h"

void build_mcb_imm(struct mcb_operand *dst, const struct yz_literal *src)
{
	dst->inner.sint = src->data.sint;
	dst->size = get_size(&src->type);
	dst->type = MCB_IMM;
}

enum MCB_SIZE get_size(const struct yz_type *type)
{
	enum YZ_TYPE raw = get_raw_type(type);
	if (yz_type_is_integer_literal(raw))
		raw = YZ_INTEGER_LITERAL_TYPE_TO_INTEGER_TYPE(raw);
	switch (raw) {
	case YZ_I8:  return MCB_I8;  case YZ_I16: return MCB_I16;
	case YZ_I32: return MCB_I32; case YZ_I64: return MCB_I64;
	case YZ_U8:  return MCB_U8;  case YZ_U16: return MCB_U16;
	case YZ_U32: return MCB_U32; case YZ_U64: return MCB_U64;
	default: break;
	}
	panicf("failed to get mcb size from '%s'",
			type_get_str(raw));
	return -1;
}
