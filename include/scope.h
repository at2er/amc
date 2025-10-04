/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef AMC_SCOPE_H
#define AMC_SCOPE_H
#include <limits.h>

enum YZ_SCOPE_TYPE {
	YZ_SCOPE_PRIVATE,
	YZ_SCOPE_PUBLIC,
	YZ_SCOPE_TYPE_COUNT
};

struct yz_scope {
	struct yz_scope *parent;
};

#endif
