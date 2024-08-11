#include "type.h"
#include "../utils/utils.h"
#include <stdlib.h>

enum GM_TYPE gm_type_get(str *s)
{
    for (int i = 0, len = LENGTH(gm_type_table); i < len; i++) {
        if (strncmp(s->s, gm_type_table[i].name, s->len) == 0)
            return gm_type_table[i].type;
    }

    return -1;
}
