/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef AMC_COMPILER_RETURN_H
#define AMC_COMPILER_RETURN_H
#include "expr.h"
#include <mcb/mcb.h>

void compile_return(struct mcb_context *mcb, const struct yz_expr *expr);

#endif
