/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "include/keywords.h"

int parse_comment(char c)
{
	if (c == ';')
		return 1;
	return 0;
}
