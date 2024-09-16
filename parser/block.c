#include "block.h"
#include "../utils/die.h"
#include "../utils/str/str.h"
#include "comment.h"
#include "file.h"
#include "unit.h"

static int block_line_parse(struct file *f, struct token *fn);
static void die_toplevel_token(struct file *f, str *unit);
static void die_undefinded_token(struct file *f, str *unit);

int block_line_parse(struct file *f, struct token *fn)
{
    struct token *token;
    str *unit = str_new();
    unit_read(unit, f);
    file_try_skip_space(f);
    if (token_find(unit, &token))
        die_undefinded_token(f, unit);
    if (token->flags.toplevel)
        die_toplevel_token(f, unit);

    token->parse_function(f, token, fn);
    if (parse_comment(f) == -2)
        die("amc: block_token_parse: multiple tokens on one line"
            "on: l:%lld c:%lld", f->cur_line, f->cur_column);

    str_free(unit);
    return 0;
}

void die_undefinded_token(struct file *f, str *unit)
{
    str_append(unit, 1, "\0");
    die("amc: block_token_parse: undefinded token: %s\n"
        "on: l:%lld c:%lld",
        unit->s, f->cur_line, f->cur_column);
}

void die_toplevel_token(struct file *f, str *unit)
{
    str_append(unit, 1, "\0");
    die("amc: block_token_parse: the token is toplevel\n"
        "on: l:%lld c:%lld",
        unit->s, f->cur_line, f->cur_column);
}

int parse_block(struct file *f, struct token *fn)
{
    while (f->src[f->pos] != '}') {
        switch (f->src[f->pos]) {
        case '\0':
            die("amc: end of file.");
            break;
        case '\n':
            goto next;
            break;
        case ' ':
            file_try_skip_space(f);
            break;
        }

        if (parse_comment(f) == -1)
            continue;

        block_line_parse(f, fn);

    next:
        file_line_next(f);
    }

    return 0;
}
