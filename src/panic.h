/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef AMC_PANIC_H
#define AMC_PANIC_H
#include "die.h"

/**
 * Panic! Panic! Panic! Yeeeeeeeeeeeh! Let's panic!
 * Panic means some error should not have happened.
 * But we need it, when you wantn't to debug the f**king lexer.
 */
#define PANIC_FMT "\x1b[41mpanic\x1b[0m: \x1b[1m%s:%s:%d\x1b[0m: "
#define PANIC_FMT_ARG __FILE__, __func__, __LINE__
#define panic(MSG) die(PANIC_FMT MSG"\n", PANIC_FMT_ARG)
#define panicf(FMT, ...) die(PANIC_FMT FMT"\n", PANIC_FMT_ARG, __VA_ARGS__)

#endif
