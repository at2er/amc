#ifndef AMC_TYPE_H
#define AMC_TYPE_H
#include "../utils/str/str.h"

enum ATOM_TYPE {
    ATOM_I8, ATOM_I16, ATOM_I32, ATOM_I64,
    ATOM_U8, ATOM_U16, ATOM_U32, ATOM_U64
};

struct {
    const char *name;
    enum ATOM_TYPE type;
} static const atom_type_table[] = {
    {"i8",  ATOM_I8 },
    {"i16", ATOM_I16},
    {"i32", ATOM_I32},
    {"i32", ATOM_I64},
    {"u8",  ATOM_U8 },
    {"u16", ATOM_U16},
    {"u32", ATOM_U32},
    {"u32", ATOM_U64},
};

enum ATOM_TYPE atom_type_get(str *s);

#endif
