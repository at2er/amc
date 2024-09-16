#ifndef AMC_TYPE_H
#define AMC_TYPE_H
#include "../utils/cint.h"
#include "../utils/str/str.h"
#include "file.h"

// TODO: AMC_ERR_TYPE and AMC_USER_TYPE name improvement.
enum ATOM_TYPE {
    AMC_ERR_TYPE, AMC_USER_TYPE, AM_NULL,
    ATOM_VOID,
    ATOM_I8, ATOM_I16, ATOM_I32, ATOM_I64,
    ATOM_U8, ATOM_U16, ATOM_U32, ATOM_U64
};

static const unsigned int AM_TYPE_OFFSET = 3;

/**
 * @field type: For builtin type.
 * @field len: The number of bytes used for type.
 */
struct am_type_group {
    const char *name;
    enum ATOM_TYPE type;
    int len;
};

union type_container {
    void *v;
    char *s;
    i32 i;
    i64 l;
};

static struct am_type_group const atom_type_table[] = {
    {"i8",  ATOM_I8,  1},
    {"i16", ATOM_I16, 2},
    {"i32", ATOM_I32, 4},
    {"i64", ATOM_I64, 8},
    {"u8",  ATOM_U8,  1},
    {"u16", ATOM_U16, 2},
    {"u32", ATOM_U32, 4},
    {"u64", ATOM_U64, 8},
};

enum ATOM_TYPE atom_type_get(str *s);

#endif
