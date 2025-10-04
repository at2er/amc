/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef AMC_IDENT_H
#define AMC_IDENT_H
#include "object.h"
#include "type.h"
#include "../utils/str/str.h"

struct yz_ident {
	str name;
	struct yz_object *object;
	struct mcb_oprand oprand;
	struct yz_type type;
};

void free_yz_ident(struct yz_ident *self);

#endif
