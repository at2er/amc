#ifndef GMC_TYPE_H
#define GMC_TYPE_H
#include "../utils/str/str.h"

enum GM_TYPE {
    GM_I8, GM_I16, GM_I32, GM_I64,
    GM_U8, GM_U16, GM_U32, GM_U64
};

struct {
    const char *name;
    enum GM_TYPE type;
} static const gm_type_table[] = {
    {"i8",  GM_I8 },
    {"i16", GM_I16},
    {"i32", GM_I32},
    {"i32", GM_I64},
    {"u8",  GM_U8 },
    {"u16", GM_U16},
    {"u32", GM_U32},
    {"u32", GM_U64},
};

enum GM_TYPE gm_type_get(str *s);

#endif
