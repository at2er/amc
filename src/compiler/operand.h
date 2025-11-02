/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef AMC_COMPILER_OPERAND_H
#define AMC_COMPILER_OPERAND_H
#include "../literal.h"
#include "../type.h"
#include <mcb/operand.h>
#include <mcb/size.h>

void get_imm_from_literal(struct mcb_operand *dst, const struct yz_literal *src);
enum MCB_SIZE get_size(const struct yz_type *type);

#endif
