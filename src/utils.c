/* SPDX-License-Identifier: MIT */
#include "utils.h"
#include <errno.h>
#include <stdint.h>
#include <string.h>

#if defined(__unix__)
#include <libgen.h>
#include <sys/stat.h>
#elif defined(__WIN32)
#include <direct.h>
#define mkdir(pathname, ...) _mkdir(pathname)
#endif

#define AMC_INT_LEN_FUNC(NAME, TYPE) \
	int NAME(TYPE src) { \
	}

int checkint(const char *str, int len)
{
	for (int i = 0; i < len; i++) {
		if (str[i] < '0' || str[i] > '9')
			return 0;
	}
	return 1;
}

int rmkdir(const char *path)
{
	const uint32_t default_filemode
		= S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
	struct stat st;
	char *parent = NULL, *path_cpy = NULL;
	int path_cpy_len, ret = 0;
	if (stat(path, &st) == 0) {
		if (S_ISDIR(st.st_mode))
			return 0;
		return 1;
	}
	path_cpy_len = strlen(path);
	path_cpy = malloc(path_cpy_len);
	strncpy(path_cpy, path, path_cpy_len);
	parent = dirname(path_cpy);
	ret = rmkdir(parent);
	free(path_cpy);
	if (ret)
		return 1;
	if (mkdir(path, default_filemode) && errno != EEXIST)
		return 1;
	return 0;
}

uint64_t ullen(uint64_t src)
{
	int len = 0;
	if (src == 0)
		return 1;
	for (uint64_t i = src; i != 0; len++)
		i /= 10;
	return len;
}

int64_t sllen(int64_t src)
{
	if (src == 0)
		return 1;
	if (src > 0)
		return ullen(src);
	return 1 + ullen(-(src));
}
