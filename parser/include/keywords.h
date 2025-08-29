/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef AMC_PARSER_KEYWORDS_H
#define AMC_PARSER_KEYWORDS_H
#include "../../include/symbol.h"

/*
 * Actually, this is the where all parse function definitions are located.
 */

/**
 * @return:
 *   0: if not a comment.
 *   1: if single line comment.
 */
int parse_comment(char c);
int parse_const(struct parser *parser);
int parse_enum(struct parser *parser);
int parse_func_call(struct parser *parser);
int parse_func_def(struct parser *parser);
int parse_func_ret(struct parser *parser);
int parse_if(struct parser *parser);
int parse_let(struct parser *parser);
int parse_match(struct parser *parser);
int parse_mod(struct parser *parser);
int parse_pub(struct parser *parser);
int parse_struct(struct parser *parser);
int parse_while(struct parser *parser);

int keyword_find(str *token, struct symbol **result);

#endif
