/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef AMC_COMPILER_FN_H
#define AMC_COMPILER_FN_H
#include "../fn.h"
#include <mcb/mcb.h>

void compile_fn_def(struct mcb_context *mcb, const struct yz_func *self);
void compile_fn_def_end(struct mcb_context *mcb);

#endif
