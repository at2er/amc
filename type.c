/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "type.h"
#include "utils.h"
#include <assert.h>
#include <string.h>

static const char *type_str[YZ_TYPE_COUNT] = {
	[YZ_I8]  = "i8",   [YZ_I16] = "i16",
	[YZ_I32] = "i32",  [YZ_I64] = "i64",
	[YZ_U8]  = "u8",   [YZ_U16] = "u16",
	[YZ_U32] = "u32",  [YZ_U64] = "u64",
	[YZ_VOID] = "void",
	[YZ_EXPR] = "expr"
};

enum YZ_TYPE convert_digit_type(enum YZ_TYPE dst, enum YZ_TYPE src)
{
	if (YZ_TYPE_IS_SIGNED_DIGIT(src) && YZ_TYPE_IS_SIGNED_DIGIT(dst))
		return MAX(dst, src);
	if (YZ_TYPE_IS_UNSIGNED_DIGIT(src) && YZ_TYPE_IS_UNSIGNED_DIGIT(dst))
		return MAX(dst, src);
	return -1;
}

void free_yz_type(struct yz_type *self)
{
	if (!self)
		return;
	free_yz_type_noself(self);
}

void free_yz_type_noself(struct yz_type *self)
{
	if (!self)
		return;
}

enum YZ_TYPE get_raw_type(const struct yz_type *self)
{
	assert(self);
	switch (self->type) {
	case YZ_EXPR: return get_raw_type(self->data.self);
	default: break;
	}
	return self->type;
}

enum YZ_TYPE type_get(const char *str, int len)
{
	for (enum YZ_TYPE res = 0; res < YZ_TYPE_COUNT; res++) {
		if (strlen(type_str[res]) != len)
			continue;
		if (strncmp(type_str[res], str, len) == 0)
			return res;
	}
	return -1;
}

const char *type_get_str(enum YZ_TYPE type)
{
	if (type >= YZ_TYPE_COUNT)
		return NULL;
	return type_str[type];
}
