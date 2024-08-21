#include "unit.h"
#include "../utils/str/str.h"

int unit_read(str *unit, struct file *f)
{
    return unit_read_until(' ', unit, f);
}

int unit_read_until(const char es, str *unit, struct file *f)
{
    for (; f->src[f->pos] != es; f->pos += 1) {
        if (f->src[f->pos] == '\0') {
            printf("amc: unit_read: end of file.\n");
            return 1;
        }

        str_append(unit, 1, &f->src[f->pos]);
    }

    return 0;
}

enum ATOM_TYPE unit_and_type_read(str *unit, struct file *f)
{
    str *type_s = str_new();
    enum ATOM_TYPE type;
    unit_read_until(':', unit, f);
    file_try_skip_space(f);
    if (f->src[f->pos] == ':') {
        f->pos += 1;
    } else {
        return -1;
    }
    file_try_skip_space(f);
    unit_read(type_s, f);
    type = atom_type_get(type_s);
    str_free(type_s);
    return type;
}
