/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "module.h"
#include <stdlib.h>

int parse_module_import(struct parser *parser)
{
	parser->imported_mod_count++;
	parser->imported_mod = realloc(parser->imported_mod,
			parser->imported_mod_count
			* sizeof(*parser->imported_mod));
	// parser->imported_mod[parser->imported_mod_count - 1] = mod;
	return 0;
}
