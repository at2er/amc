#ifndef AMC_FILE_H
#define AMC_FILE_H
#include "../utils/cint.h"

struct file {
    i64 cur_line;
    i64 cur_column;
    i64 pos;
    i64 len;

    u32 indent;

    char *src;
    const char *path;
};

int file_init(const char *path, struct file *self);
int file_line_next(struct file *self);
int file_pos_next(struct file *self);
int file_pos_nnext(int n, struct file *self);
int file_skip_space(struct file *self);
int file_try_skip_space(struct file *self);

#endif
