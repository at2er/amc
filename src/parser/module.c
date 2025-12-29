/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "module.h"
#include "../darr.h"
#include <stdlib.h>

int parse_module_import(struct parser *parser)
{
	darr_append(parser->imported_mod, parser->imported_mod_count, NULL);
	return 0;
}
