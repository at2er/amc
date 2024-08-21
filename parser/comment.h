#ifndef AMC_COMMENT_H
#define AMC_COMMENT_H
#include "file.h"

/**
 * @return:
 *   -1: if single line comment.
 *   -2: if multiline comment.
 *   1: if incomplete comment character(invaild).
 *   2: if not a comment.
 */
int parse_comment(struct file *f);
int parse_multiline_comment(struct file *f);
int parse_single_comment(struct file *f);

#endif
