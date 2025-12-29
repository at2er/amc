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
	[YZ_I8_LITERAL]  = "i8_literal",
	[YZ_I16_LITERAL] = "i16_literal",
	[YZ_I32_LITERAL] = "i32_literal",
	[YZ_I64_LITERAL] = "i64_literal",
	[YZ_U8_LITERAL]  = "u8_literal",
	[YZ_U16_LITERAL] = "u16_literal",
	[YZ_U32_LITERAL] = "u32_literal",
	[YZ_U64_LITERAL] = "u64_literal",
	[YZ_EXPR] = "expr",
	[YZ_FUNC_CALL] = "func_call",
	[YZ_IDENT_LITERAL] = "ident_literal"
};

enum YZ_TYPE convert_digit_type(enum YZ_TYPE dst, enum YZ_TYPE src)
{
	if (yz_type_is_signed_integer(src) && yz_type_is_signed_integer(dst))
		return MAX(dst, src) == dst ? dst : -1;
	if (yz_type_is_unsigned_integer(src)
			&& yz_type_is_unsigned_integer(dst))
		return MAX(dst, src) == dst ? dst : -1;
	if (yz_type_is_integer_literal(dst))
		return src;
	if (yz_type_is_integer_literal(src))
		return dst;
	return -1;
}

const struct yz_type *convert_type_implicity(const struct yz_type *dst,
		const struct yz_type *src)
{
	const struct yz_type *lrc, *rrc;
	enum YZ_TYPE tmp;
	lrc = get_raw_type_container(dst);
	rrc = get_raw_type_container(src);
	if (!yz_type_is_integer(lrc->type) || !yz_type_is_integer(rrc->type))
		return NULL;
	tmp = convert_digit_type(lrc->type, rrc->type);
	if (tmp == -1)
		return NULL;
	return tmp == dst->type ? dst : NULL;
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
	const struct yz_type *raw;
	assert(self);
	assert(raw = get_raw_type_container(self));
	return self->type;
}

const struct yz_type *get_raw_type_container(const struct yz_type *self)
{
	assert(self);
	switch (self->type) {
	case YZ_EXPR:
	case YZ_FUNC_CALL:
	case YZ_IDENT_LITERAL:
		return get_raw_type_container(self->data.self);
	default: break;
	}
	return self;
}

const struct yz_type *get_sum_type(
		const struct yz_type *lhs,
		const struct yz_type *rhs)
{
	const struct yz_type *lrc, *rrc;
	enum YZ_TYPE tmp;

	lrc = get_raw_type_container(lhs);
	rrc = get_raw_type_container(rhs);
	if (!yz_type_is_integer(lrc->type) || !yz_type_is_integer(rrc->type))
		return NULL;

	tmp = convert_digit_type(lrc->type, rrc->type);
	if (tmp == -1)
		return NULL;
	return tmp == lrc->type ? lrc : rrc;
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

bool yz_type_is_integer(enum YZ_TYPE type)
{
	if (type >= YZ_I8 && type <= YZ_U64)
		return true;
	if (yz_type_is_integer_literal(type))
		return true;
	return false;
}

bool yz_type_is_integer_literal(enum YZ_TYPE type)
{
	return type >= YZ_I8_LITERAL && type <= YZ_U64_LITERAL;
}

bool yz_type_is_signed_integer(enum YZ_TYPE type)
{
	if (type >= YZ_I8 && type <= YZ_I64)
		return true;
	if (type >= YZ_I8_LITERAL && type <= YZ_I64_LITERAL)
		return true;
	return false;
}

bool yz_type_is_unsigned_integer(enum YZ_TYPE type)
{
	if (type >= YZ_U8 && type <= YZ_U64)
		return true;
	if (type >= YZ_U8_LITERAL && type <= YZ_U64_LITERAL)
		return true;
	return false;
}
