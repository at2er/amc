/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "compiler/fn.h"
#include "compiler/panic.h"
#include <mcb/mcb.h>

static void get_arg_oprands(const struct yz_func *self,
		struct mcb_oprand **result);

void get_arg_oprands(const struct yz_func *self, struct mcb_oprand **result)
{
	for (int i = 0; i < self->argc; i++)
		result[i] = &self->args[i]->oprand;
}

void compile_fn_def(struct mcb_context *mcb, const struct yz_func *self)
{
	struct mcb_oprand *args[self->argc + 1];
	args[self->argc] = NULL;
	get_arg_oprands(self, args);
	if (MCB_CALL(enter_func)(mcb, 0, args, MCB_I64, self->path.s))
		PANIC_MCB_CALL;
}

void compile_fn_def_end(struct mcb_context *mcb)
{
	if (MCB_CALL(exit_func)(mcb))
		PANIC_MCB_CALL;
}
