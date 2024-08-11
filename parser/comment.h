#ifndef GMC_COMMENT_H
#define GMC_COMMENT_H
#include "file.h"

int parse_comment(struct file *f);
int parse_multiline_comment(struct file *f);
int parse_single_comment(struct file *f);

#endif
