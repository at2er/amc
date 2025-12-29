/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef AMC_FLAGS_H
#define AMC_FLAGS_H
#include <stdint.h>

#define test_amc_flags(FLAG) (amc_flags & FLAG)

enum {
	AMC_FLAGS_DEBUG = 1,
	AMC_FLAGS_STDOUT_MODE  = 1 << 1,
	AMC_FLAGS_COMPILE_ONLY = 1 << 2,
	AMC_FLAGS_NO_CACHE     = 1 << 3
};

extern uint32_t amc_flags;

#endif
