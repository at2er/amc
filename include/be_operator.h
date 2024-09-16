#ifndef AMC_BE_OPERATOR_H
#define AMC_BE_OPERATOR_H
#include "../parser/file.h"
/**
 * For "parser/expr".
 */

int backend_cmd_add(struct file *f);
int backend_cmd_sub(struct file *f);
int backend_cmd_div(struct file *f);
int backend_cmd_mul(struct file *f);

#endif
