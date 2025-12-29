/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef AMC_COMPILER_CACHE_H
#define AMC_COMPILER_CACHE_H
#include "../str.h"

struct global_cache {
	str dir;
};

extern struct global_cache amc_global_cache;

void init_global_cache(struct global_cache *self,
		const char *cwd,
		char *dirpath);

#endif
