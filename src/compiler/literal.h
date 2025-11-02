/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef AMC_COMPILER_LITERAL_H
#define AMC_COMPILER_LITERAL_H
#include "../literal.h"
#include <mcb/mcb.h>
#include <mcb/operand.h>

void compile_literal(struct mcb_context *mcb,
		struct mcb_operand *self,
		const struct yz_literal *src);

#endif
