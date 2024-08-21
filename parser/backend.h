#ifndef AMC_BACKEND_H
#define AMC_BACKEND_H
#include "file.h"

int comp_func(struct file *f);
int comp_struct(struct file *f);
int comp_const(struct file *f);
int comp_var(struct file *f);
int comp_let(struct file *f);
int comp_match(struct file *f);
int comp_if(struct file *f);
int comp_elif(struct file *f);
int comp_else(struct file *f);
int comp_return(struct file *f);

int comp_cmd_add(struct file *f);
int comp_cmd_sub(struct file *f);
int comp_cmd_div(struct file *f);
int comp_cmd_mul(struct file *f);

#endif
