/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef AMC_VAL_H
#define AMC_VAL_H
#include "type.h"
#include "../utils/cint.h"

union yz_val_data {
	void *v;
	char *s;
	i8 b;
	i16 w;
	i32 i;
	i64 l;

	struct expr *expr;
	struct symbol *sym;
};

typedef struct yz_val {
	union yz_val_data data;
	yz_type type;
} yz_val;

union yz_extract_val_data {
	int index;
	yz_val *offset;
};

enum YZ_EXTRACT_VAL_TYPE {
	YZ_EXTRACT_ARRAY,
	YZ_EXTRACT_STRUCT,
	YZ_EXTRACT_STRUCT_FROM_PTR
};

typedef struct yz_extract_val {
	union yz_extract_val_data data;
	struct symbol *elem, *sym;
	enum YZ_EXTRACT_VAL_TYPE type;
} yz_extract_val;

struct symbol *yz_get_extracted_val(yz_extract_val *val);

void free_yz_extract_val(struct yz_extract_val *self);
void free_yz_val(yz_val *self);
void free_yz_val_noself(yz_val *self);

#endif
