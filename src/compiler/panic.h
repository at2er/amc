/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef AMC_COMPILER_PANIC_H
#define AMC_COMPILER_PANIC_H
#include "../die.h"
#include "../panic.h"

#define PANIC_MCB_CALL die(PANIC_FMT"failed to call mcb\n", __FILE__, __func__)

#endif
