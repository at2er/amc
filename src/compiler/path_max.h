#ifndef AMC_COMPILER_PATH_MAX_H
#define AMC_COMPILER_PATH_MAX_H

#if defined(__linux__)
#include <linux/limits.h>
#else
#error unsupport platform
#endif

#endif
