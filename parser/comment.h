#ifndef AMC_COMMENT_H
#define AMC_COMMENT_H
#include "file.h"

/**
 * @return:
 *   0: if not a comment.
 *   1: if single line comment.
 */
int parse_comment(struct file *f);

#endif
