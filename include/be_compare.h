#ifndef AMC_BE_COMPARE_H
#define AMC_BE_COMPARE_H
#include "../parser/file.h"

int backend_if(struct file *f);
int backend_elif(struct file *f);
int backend_else(struct file *f);
int backend_match(struct file *f);

#endif
