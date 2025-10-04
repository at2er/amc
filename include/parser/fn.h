/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef AMC_PARSER_FN_H
#define AMC_PARSER_FN_H
#include "parser.h"
#include "scope.h"

int parse_func_def(struct parser *parser, enum YZ_SCOPE_TYPE scope_of);

#endif
