/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef AMC_TYPE_H
#define AMC_TYPE_H
#include <stdbool.h>

#define YZ_INTEGER_LITERAL_TYPE_TO_INTEGER_TYPE(TYPE) ((TYPE) - YZ_I8_LITERAL)

enum YZ_TYPE {
	YZ_I8, YZ_I16, YZ_I32, YZ_I64,
	YZ_U8, YZ_U16, YZ_U32, YZ_U64,
	YZ_VOID,

	YZ_I8_LITERAL,  YZ_I16_LITERAL,
	YZ_I32_LITERAL, YZ_I64_LITERAL,
	YZ_U8_LITERAL,  YZ_U16_LITERAL,
	YZ_U32_LITERAL, YZ_U64_LITERAL,

	YZ_EXPR,
	YZ_FUNC_CALL,
	YZ_IDENT_LITERAL,

	YZ_TYPE_COUNT
};

union yz_type_data {
	const struct yz_type *self;
};

struct yz_type {
	union yz_type_data data;
	enum YZ_TYPE type;
};

/**
 * @return: `YZ_I8` - `YZ_I64` and `YZ_U8` - `YZ_U64` on success,
 *          -1 when type cannot convert, e.g. unsigned digit to signed digit.
 */
enum YZ_TYPE convert_digit_type(enum YZ_TYPE dst, enum YZ_TYPE src);

const struct yz_type *convert_type_implicity(const struct yz_type *dst,
		const struct yz_type *src);

void free_yz_type(struct yz_type *self);

void free_yz_type_noself(struct yz_type *self);

enum YZ_TYPE get_raw_type(const struct yz_type *self);
const struct yz_type *get_raw_type_container(const struct yz_type *self);

const struct yz_type *get_sum_type(
		const struct yz_type *lhs,
		const struct yz_type *rhs);

/**
 * @return: `enum YZ_TYPE` on success, -1 on not found.
 */
enum YZ_TYPE type_get(const char *str, int len);

const char *type_get_str(enum YZ_TYPE type);

bool yz_type_is_integer(enum YZ_TYPE type);
bool yz_type_is_integer_literal(enum YZ_TYPE type);
bool yz_type_is_signed_integer(enum YZ_TYPE type);
bool yz_type_is_unsigned_integer(enum YZ_TYPE type);

#endif
