/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "include/imm.h"
#include "../../utils/utils.h"
#include <stdio.h>

str *asf_imm_str_new(struct asf_imm *imm)
{
	str *s = str_new();
	str_expand(s, ullen(imm->data.iq) + 2);
	snprintf(s->s, s->len, "$%ld", imm->data.iq);
	return s;
}
