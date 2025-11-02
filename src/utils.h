/* SPDX-License-Identifier: MIT */
#ifndef AMC_UTILS_H
#define AMC_UTILS_H
#include <stdint.h>
#include <stdlib.h>

#define CHR_IS_NUM(x)         (REGION_INT((x), '0', '9'))
#define LENGTH(x)             (sizeof((x)) / (sizeof((x)[0])))
#define MAX(a, b)             ((a) > (b) ? (a) : (b))
#define MIN(a, b)             ((a) < (b) ? (a) : (b))
#define REGION_INT(val, a, b) ((val) >= (a) && (val) <= (b))
#define free_cl(ptr)          free((ptr)); (ptr) = NULL
#define free_safe(ptr)        if ((ptr) != NULL) { free_cl((ptr)); }

#define ERROR_STR "\x1b[31merror\x1b[0m"
#define HINT_STR "\x1b[34mhint\x1b[0m"

#define PRINT_SHIFT 4

/**
 * @param len: string len
 * @return:
 *   1: when all characters were integer.
 *   0: when had not number character.
 */
int checkint(const char *str, int len);
int rmkdir(const char *path);
uint64_t ullen(uint64_t src);
int64_t sllen(int64_t src);

#endif
