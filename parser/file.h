#ifndef GMC_FILE_H
#define GMC_FILE_H

struct file {
    long long cur_line;
    long long cur_column;
    long long pos;
    long long len;
    char *src;
    const char *path;
};

int file_init(const char *path, struct file *self);
int file_pos_next(struct file *self);
int file_skip(struct file *self);
int file_try_skip(struct file *self);

#endif
