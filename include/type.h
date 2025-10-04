/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef AMC_TYPE_H
#define AMC_TYPE_H

#define YZ_TYPE_IS_DIGIT(TYPE)          ((TYPE) >= YZ_I8 && (TYPE) <= YZ_U64)
#define YZ_TYPE_IS_SIGNED_DIGIT(TYPE)   ((TYPE) >= YZ_I8 && (TYPE) <= YZ_I64)
#define YZ_TYPE_IS_UNSIGNED_DIGIT(TYPE) ((TYPE) >= YZ_U8 && (TYPE) <= YZ_U64)

enum YZ_TYPE {
	YZ_I8, YZ_I16, YZ_I32, YZ_I64,
	YZ_U8, YZ_U16, YZ_U32, YZ_U64,
	YZ_VOID,

	YZ_EXPR,

	YZ_TYPE_COUNT
};

union yz_type_data {
	struct yz_type *self;
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
void free_yz_type(struct yz_type *self);
void free_yz_type_noself(struct yz_type *self);
enum YZ_TYPE get_raw_type(const struct yz_type *self);
/**
 * @return: `enum YZ_TYPE` on success, -1 on not found.
 */
enum YZ_TYPE type_get(const char *str, int len);
const char *type_get_str(enum YZ_TYPE type);

#endif
