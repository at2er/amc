/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef AMC_COMPILER_OPRAND_H
#define AMC_COMPILER_OPRAND_H
#include "type.h"
#include <mcb/size.h>

enum MCB_SIZE get_size(const struct yz_type *type);
enum MCB_SIZE get_size_raw(enum YZ_TYPE type);

#endif
