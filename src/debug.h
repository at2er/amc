/* SPDX-License-Identifier: MIT */
#ifndef AMC_DEBUG_H
#define AMC_DEBUG_H
#include "flags.h"
#include <stdio.h>

#ifdef NODBUG
#define debug_print(...)
#define debug_printf(...)
#else /* NODBUG */
#define DEBUG_FMT "[\x1b[34mdebug\x1b[0m:%s:%d]: "

#define if_in_debug if (amc_flags & AMC_FLAGS_DEBUG)

#define debug_print(FMT) if_in_debug \
		printf(DEBUG_FMT FMT "\n", __FILE__, __LINE__);

#define debug_printf(FMT, ...) if_in_debug \
		printf(DEBUG_FMT FMT "\n", __FILE__, __LINE__, __VA_ARGS__);
#endif /* NODBUG */

#endif
