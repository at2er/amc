#include "parser.h"
#include "../utils/str/str.h"
#include "../utils/die.h"
#include "comment.h"
#include "unit.h"
#include "token.h"

int parser_init(const char* path, struct file* f)
{
    if (file_init(path, f))
        die("amc: file_init: no such file: %s\n", path);

    while (f->src[f->pos] != '\0') {
        file_try_skip_space(f);
        if (f->src[f->pos] == '\n')
            goto next;
        parse_comment(f);

        str *unit = str_new();
        struct token *token = NULL;
        unit_read(unit, f);
        if (token_find(unit, &token)) {
            str_append(unit, 1, "\0");
            die("amc: token_find: not found token: %s\n"
                "l:%lld c:%lld |\n", unit->s, f->cur_line, f->cur_column);
        }
        token->parse_function(f);

    free_unit:
        str_free(unit);
    next:
        file_line_next(f);
    }

    return 0;
}
