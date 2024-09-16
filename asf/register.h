#ifndef AMC_BE_REGISTER_H
#define AMC_BE_REGISTER_H
#include "../parser/type.h"

/**
 * @field bit_64: For 32-bit platform when is 0.
 *                For 64-bit platform when is 1.
 * @field common: Without any special purpose.
 */
struct reg_flags {
    unsigned int bit_64:1, common:1, used:1;
};

struct reg {
    const char *name;
    struct reg_flags flags;
};

static struct reg regs[] = {
    {"rax", {1, 1, 0}},
    {"rbx", {1, 1, 0}},
    {"rcx", {1, 1, 0}},
    {"rdx", {1, 1, 0}},
    {"rbp", {1, 0, 0}},
    {"rsi", {1, 1, 0}},
    {"rdi", {1, 1, 0}},
    {"rsp", {1, 0, 0}},

    // only in 64-bit
    {"r8",  {1, 1, 0}},
    {"r9",  {1, 1, 0}},
    {"r10", {1, 1, 0}},
    {"r11", {1, 1, 0}},
    {"r12", {1, 1, 0}},
    {"r13", {1, 1, 0}},
    {"r14", {1, 1, 0}},
    {"r15", {1, 1, 0}},

    {"eax", {0, 1, 0}},
    {"ebx", {0, 0, 0}},
    {"ecx", {0, 1, 0}},
    {"edx", {0, 1, 0}},
    {"ebp", {0, 0, 0}},
    {"esi", {0, 0, 0}},
    {"edi", {0, 0, 0}},
    {"esp", {0, 0, 0}},
};

static const unsigned int AM_BE_REG_32_OFFSET = 16;
static const unsigned int AM_BE_REG_64_OFFSET = 0;

char mov_len_get(struct reg *reg);
struct reg *reg_get(enum ATOM_TYPE type);

#endif
