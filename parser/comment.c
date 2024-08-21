#include "comment.h"
#include "../utils/die.h"
#include <stdio.h>

int parse_comment(struct file *f)
{
    if (f->src[f->pos] == '/') {
        file_pos_next(f);
        switch (f->src[f->pos]) {
        case '/':
            parse_single_comment(f);
            return -1;
            break;
        case '*':
            file_pos_next(f);
            parse_multiline_comment(f);
            return -2;
            break;
        default:
            die("amc: parse_comment: incomplete comment character: %c", f->src[f->pos]);
            return 1;
            break;
        }
    }

    return 2;
}

int parse_multiline_comment(struct file *f)
{
    while (f->src[f->pos] != '\0') {
        if (f->src[f->pos] == '*'
            && f->src[f->pos + 1] == '/')
            break;

        file_pos_next(f);
    }

    if (f->src[f->pos] == '\0')
        die("amc: parse_multiline_comment: end of file.");

    file_pos_nnext(2, f);

    return 0;
}

int parse_single_comment(struct file *f)
{
    return file_line_next(f);
}
