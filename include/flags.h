/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef AMC_FLAGS_H
#define AMC_FLAGS_H

#define DEBUG_FMT "[\x1b[34mdebug\x1b[0m] "

struct amc_flags_t {
	unsigned int debug:1;
};

extern struct amc_flags_t amc_flags;

#endif
