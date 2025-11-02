/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef AMC_KEYWORD_H
#define AMC_KEYWORD_H

enum KEYWORDS {
	KEYWORD_ELSE,
	KEYWORD_ENUM,
	KEYWORD_FN,
	KEYWORD_IF,
	KEYWORD_LET,
	KEYWORD_MATCH,
	KEYWORD_MOD,
	KEYWORD_MUT,
	KEYWORD_PUB,
	KEYWORD_RETURN,
	KEYWORD_STRUCT,
	KEYWORD_WHILE,
	KEYWORDS_COUNT
};

/**
 * @return: `enum KEYWORDS` on success, -1 when not found.
 */
enum KEYWORDS keyword_get(const char *str, int len);

#endif
