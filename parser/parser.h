#ifndef AMC_PARSER_H
#define AMC_PARSER_H
#include "file.h"

struct parser {
    unsigned int has_err:1, has_main:1, in_main:1;
};

extern struct parser parser_global_conf;

int parser_init(const char* path, struct file* f);

#endif
