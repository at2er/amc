#include "indent.h"
#include "../utils/die.h"

int indent_get(struct file *f)
{
    if (f->src[f->pos] != '\n') {
        die("amc: indent_get: %c is not break line character\n"
            "l:%lld c:%lld |\n",
            f->src[f->pos],
            f->cur_line, f->cur_column);
    }

    f->pos += 1;
    f->cur_column += 1;

    if (f->src[f->pos] != '\n') {
        return 0;
    }

    int i = 0;
    while (f->src[f->pos] == ' '
        || f->src[f->pos] == '\t') {
        f->pos += 1;
        i += 1;
    }

    if (f->src[f->pos] == '\n') {
        return indent_get(f);
    }

    return i;
}
