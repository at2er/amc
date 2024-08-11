#include "parser.h"
#include "file.h"
#include "comment.h"

int parser_init(const char* path, struct file* f)
{
    file_init(path, f);

    while (f->src[f->pos] != '\0') {
        parse_comment(f);

    }

    return 0;
}
