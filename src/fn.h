/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef AMC_FN_H
#define AMC_FN_H
#include "ident.h"
#include "literal.h"
#include "type.h"
#include "str.h"
#include <stdint.h>

struct yz_func {
	str name, path;
	struct yz_type type;

	/**
	 * Why use more arguments for your function? uint16_t has the f**king
	 * UINT16_MAX(65535), your shit has 65535 arguments??? Please fix
	 * your shits! X(
	 */
	uint16_t argc;
	struct yz_ident **args;
};

struct yz_func_call {
	uint16_t argc;
	struct yz_literal **args;
	const struct yz_func *callee;
};

void free_yz_func(struct yz_func *self);
void free_yz_func_call(struct yz_func_call *self);
void print_yz_func_call(const struct yz_func_call *self, uint16_t depth);

#endif
