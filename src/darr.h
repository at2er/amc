/* SPDX-License-Identifier: MIT */
#ifndef DARR_H
#define DARR_H
#include <stdlib.h>

#define darr_append(SELF, COUNT, ELEM) \
	do { \
		darr_expand((SELF), (COUNT)); \
		(SELF)[(COUNT) - 1] = (ELEM); \
	} while (0)

#define darr_expand(SELF, COUNT) \
	do { \
		(COUNT)++; \
		(SELF) = realloc((SELF), (COUNT) * sizeof(*(SELF))); \
	} while (0)

#endif
