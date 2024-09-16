#include "type.h"
#include "../utils/utils.h"

enum ATOM_TYPE atom_type_get(str *s)
{
    for (int i = 0, len = LENGTH(atom_type_table); i < len; i++) {
        if (strncmp(s->s, atom_type_table[i].name, s->len) == 0)
            return atom_type_table[i].type;
    }

    return AMC_ERR_TYPE;
}
