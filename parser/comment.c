#include "comment.h"
#include <stdio.h>

int parse_comment(struct file *f)
{
    if (f->src[f->pos] == '/') {
        f->pos += 1;
        switch (f->src[f->pos]) {
        case '/':
            parse_single_comment(f);
            break;
        case '*':
            parse_multiline_comment(f);
            break;
        default:
            return 1;
            break;
        }
    } else {
        return 2;
    }

    return 0;
}

int parse_multiline_comment(struct file *f)
{
    while (f->src[f->pos] != '*') {
        if (f->src[f->pos] == '\0') {
            printf("no end of multiline comment\n");
            return 1;
        }
        f->pos += 1;
    }
    f->pos += 2;

    return 0;
}

int parse_single_comment(struct file *f)
{
    while (f->src[f->pos] != '\n')
        f->pos += 1;
    f->pos += 1;

    return 0;
}
