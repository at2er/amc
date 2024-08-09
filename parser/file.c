#include "file.h"
#include <stdio.h>
#include <stdlib.h>

int file_init(const char *path, struct file *self)
{
    FILE* stream = fopen(path, "r");
    if (stream == NULL)
        return 1;
    fseek(stream, 0, SEEK_END);
    self->cur_line = 0;
    self->cur_column = 0;
    self->pos = 0;
    self->len = ftell(stream);
    self->path = path;
    self->src = calloc(self->len + 1, sizeof(char));
    fseek(stream, 0, SEEK_SET);
    fread(self->src, self->len, 1, stream);
    fclose(stream);
    return 0;
}

int file_pos_next(struct file *self)
{
    if (self->src[self->pos] == '\n') {
        self->cur_line += 1;
        self->cur_column = 0;
    } else {
        self->cur_column += 1;
    }

    self->pos += 1;

    return 0;
}

int file_skip(struct file *self)
{
    return 0;
}

int file_try_skip(struct file *self)
{
    return 0;
}
