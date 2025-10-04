/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef AMC_PARSER_UTILS_H
#define AMC_PARSER_UTILS_H

#define ERR_FMT "\x1b[31merror\x1b[0m: \x1b[1m%s\x1b[0m: %lu,%lu: "
#define ERR_FMT_ARG(PARSER) \
	(PARSER)->lexer.self.fpath, \
	(PARSER)->lexer.self.line, \
	(PARSER)->lexer.self.column

#endif
