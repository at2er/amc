/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "include/cache.h"
#include "../include/parser.h"
#include "../utils/str/str.h"
#include "../utils/utils.h"
#include <stdio.h>
#include <string.h>

#define cache_dir_name "amc"
#define cache_dir_path ".cache"

int cache_dir_create(str *root_dir)
{
	str dir = STR_EMPTY;
	if (global_parser.target_path.s != NULL)
		return 0;
	str_expand(&dir, root_dir->len
			+ strlen(cache_dir_name)
			+ strlen(cache_dir_path)
			+ 3);
	snprintf(dir.s, dir.len, "%s/%s/%s", root_dir->s,
			cache_dir_path,
			cache_dir_name);
	if (rmkdir(dir.s))
		return 1;
	global_parser.target_path.s = dir.s;
	global_parser.target_path.len = strlen(dir.s);
	return 0;
}
