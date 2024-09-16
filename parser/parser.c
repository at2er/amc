#include "parser.h"
#include "../include/backend.h"
#include "../utils/die.h"
#include "../utils/str/str.h"
#include "comment.h"
#include "token.h"
#include "unit.h"

struct parser parser_global_conf = {0, 0, 0};

int parser_init(const char *path, struct file *f)
{
    if (file_init(path, f))
        die("amc: file_init: no such file: %s\n", path);
    if (backend_file_new(f))
        die("amc: backend_file_new: cannot create new file");

    while (f->src[f->pos] != '\0') {
        file_try_skip_space(f);
        if (f->src[f->pos] == '\n') {
            goto next;
        } else if (parse_comment(f) == -1) {
            continue;
        }

        str *unit = str_new();
        struct token *token = NULL;
        unit_read(unit, f);
        if (token_find(unit, &token)) {
            str_append(unit, 1, "\0");
            die("amc: token_find: not found token: %s\n"
                "l:%lld c:%lld |\n",
                unit->s, f->cur_line, f->cur_column);
        }
        token->parse_function(f, token, NULL);

        str_free(unit);
    next:
        file_line_next(f);
    }

    backend_end();

    return 0;
}
