/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "compiler/oprand.h"
#include "die.h"
#include "panic.h"
#include "type.h"

enum MCB_SIZE get_size(const struct yz_type *type)
{
	switch (type->type) {
	case YZ_EXPR: return get_size(type->data.self);
	default: break;
	}
	return get_size_raw(type->type);
}

enum MCB_SIZE get_size_raw(enum YZ_TYPE type)
{
	switch (type) {
	case YZ_I8:  return MCB_I8;  case YZ_I16: return MCB_I16;
	case YZ_I32: return MCB_I32; case YZ_I64: return MCB_I64;
	case YZ_U8:  return MCB_U8;  case YZ_U16: return MCB_U16;
	case YZ_U32: return MCB_U32; case YZ_U64: return MCB_U64;
	default: break;
	}
	die(PANIC_FMT"failed to get mcb size from YZ_TYPE '%s'\n",
			PANIC_FMT_ARG,
			type_get_str(type));
	return -1;
}
