#include "block.h"
#include "../utils/die.h"
#include "../utils/str/str.h"
#include "comment.h"
#include "file.h"
#include "token.h"
#include "unit.h"

static int block_blank_line_check(struct file *f);
static int block_line_parse(struct file *f, str *unit);

int block_blank_line_check(struct file *f)
{
    if (f->indent != 0)
        return 0;
    if (f->src[f->pos] != ' '
        && f->src[f->pos] != '\t')
        return 0;

    return 1;
}

int block_line_parse(struct file *f, str *unit)
{
    struct token *token;
    unit_read(unit, f);
    if (token_find(unit, &token)) {
        str_append(unit, 1, "\0");
        die("amc: block_token_parse: undefinded token: %s"
            "in: l:%lld c:%lld",
            unit->s,
            f->cur_line, f->cur_column);
    }
    token->parse_function(f);
    if (parse_comment(f) == -2)
        die("amc: block_token_parse: multiple tokens");

    return 0;
}

int parse_block(struct file *f)
{
    while (f->src[f->pos] != '\0') {
        if (block_blank_line_check(f))
            goto next;
        str *unit = str_new();
        block_line_parse(f, unit);

    next:
        file_line_next(f);
    free_unit:
        str_free(unit);
    }

    return 0;
}
