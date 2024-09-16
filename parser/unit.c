#include "unit.h"
#include "../utils/str/str.h"

int unit_read(str *unit, struct file *f)
{
    return unit_read_until(" ", unit, f);
}

int unit_read_until(const char *es, str *unit, struct file *f)
{
    while (f->src[f->pos] != '\0') {
        for (int i = 0; es[i] != '\0'; i++) {
            if (f->src[f->pos] == es[i])
                return 0;
        }

        str_append(unit, 1, &f->src[f->pos]);
        file_pos_next(f);
    }

    return 0;
}

enum ATOM_TYPE unit_and_type_read(str *unit, struct file *f)
{
    str *type_s = str_new();
    enum ATOM_TYPE type;
    unit_read_until(": ", unit, f);
    file_try_skip_space(f);
    if (f->src[f->pos] == ':') {
        f->pos += 1;
    } else {
        return AMC_ERR_TYPE;
    }
    file_try_skip_space(f);
    unit_read_until(" (", type_s, f);
    type = atom_type_get(type_s);
    str_free(type_s);
    return type;
}
