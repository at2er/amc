/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef AMC_FN_H
#define AMC_FN_H
#include "ident.h"
#include "type.h"
#include "../utils/str/str.h"
#include <stdint.h>

struct yz_func {
	str name, path;
	struct yz_type type;

	/**
	 * Why use more arguments for your function? uint16_t has the f**king
	 * UINT16_MAX(65535), your shit has 65535 arguments??? Please fix
	 * your shits! X/
	 */
	uint16_t argc;
	struct yz_ident **args;
};

void free_yz_func(struct yz_func *self);

#endif
