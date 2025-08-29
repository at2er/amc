/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef AMC_BE_ASF_REGISTER_H
#define AMC_BE_ASF_REGISTER_H
#include "bytes.h"
#include "../../../utils/str/str.h"

enum ASF_REG_PURPOSE_TYPE {
	ASF_REG_PURPOSE_NULL,
	ASF_REG_PURPOSE_FUNC_RESULT,
	ASF_REG_PURPOSE_FUNC_ARG,
	ASF_REG_PURPOSE_EXPR_RESULT,
	ASF_REG_PURPOSE_STORAGE
};

enum ASF_REGS {
	ASF_REG_RAX,
	ASF_REG_RBX,
	ASF_REG_RCX,
	ASF_REG_RDX,
	ASF_REG_RBP,
	ASF_REG_RSI,
	ASF_REG_RDI,
	ASF_REG_RSP,

	ASF_REG_R8,
	ASF_REG_R9,
	ASF_REG_R10,
	ASF_REG_R11,
	ASF_REG_R12,
	ASF_REG_R13,
	ASF_REG_R14,
	ASF_REG_R15,

	ASF_REG_EAX,
	ASF_REG_EBX,
	ASF_REG_ECX,
	ASF_REG_EDX,
	ASF_REG_EBP,
	ASF_REG_ESI,
	ASF_REG_EDI,
	ASF_REG_ESP
};

/**
 * @field bit_64: For 32-bit platform when is 0.
 *                For 64-bit platform when is 1.
 * @field common: Without any special purpose.
 */
struct reg_flags {
	unsigned int bit_64:1, common:1;
};

struct asf_reg {
	const char *name;
	int bytes;
	struct reg_flags flags;
	enum ASF_REG_PURPOSE_TYPE *purpose;
};

extern struct asf_reg asf_regs[];

static const unsigned int ASF_REG_32_OFFSET = 16;
static const unsigned int ASF_REG_64_OFFSET = 0;

str *asf_reg_clean(enum ASF_REGS reg);
str *asf_reg_get_str(struct asf_reg *reg);
enum ASF_REGS asf_reg_get(enum ASF_BYTES type);
int asf_regs_init(void);

#endif
