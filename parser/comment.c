#include "comment.h"

int parse_comment(struct file *f)
{
    if (f->src[f->pos] == ';') {
        return file_line_next(f);
    }

    return 2;
}
