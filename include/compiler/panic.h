/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef AMC_COMPILER_PANIC_H
#define AMC_COMPILER_PANIC_H
#include "../panic.h"
#include "../../utils/die.h"

#define PANIC_MCB_CALL panic_mcb_call(__FILE__, __func__)

static inline void panic_mcb_call(const char *file, const char *func)
{
	die(PANIC_FMT"failed to call mcb\n", file, func);
}

#endif
