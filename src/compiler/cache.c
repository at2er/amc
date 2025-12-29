/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "cache.h"
#include "path_max.h"
#include "../debug.h"
#include "../panic.h"
#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __unix__
#include <sys/stat.h>
#else
#error unsupport platform
#endif

#define DEFAULT_AMC_CACHE_DIR "amc"
#define DEFAULT_CACHE_DIR ".cache"

#define DEFAULT_AMC_CACHE_DIR_FULL \
	DEFAULT_CACHE_DIR"/"DEFAULT_AMC_CACHE_DIR

static int cat_path(char *dest, size_t dest_len,
		const char *s0, const char *s1);
static void emkdir(const char *path, __mode_t mode, const char *dirname);

static const unsigned int default_filemode
		= S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;

int cat_path(char *dest, size_t dest_len, const char *s0, const char *s1)
{
	snprintf(dest, dest_len, "%s/%s", s0, s1);
	return 0;
}

void emkdir(const char *path, __mode_t mode, const char *dirname)
{
	if (!mkdir(path, mode)) {
		debug_printf("mkdir '%s'", dirname);
		return;
	}
	if (errno != EEXIST)
		panicf("failed to mkdir '%s'", dirname);
}

void init_global_cache(struct global_cache *self,
		const char *cwd,
		char *dirpath)
{
	char *path = NULL;
	assert(self);
	memset(self, 0, sizeof(*self));

	path = calloc(PATH_MAX + 1, sizeof(char));

	cat_path(path, PATH_MAX, cwd, DEFAULT_CACHE_DIR);
	emkdir(path, default_filemode, DEFAULT_CACHE_DIR);

	cat_path(path, PATH_MAX, cwd, DEFAULT_AMC_CACHE_DIR_FULL);
	emkdir(path, default_filemode, DEFAULT_AMC_CACHE_DIR_FULL);

	self->dir.s = path;
	self->dir.len = strlen(self->dir.s);
}
