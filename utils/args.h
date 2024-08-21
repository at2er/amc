#ifndef ARGS_H
#define ARGS_H
#include "str/str.h"

struct option {
    const char *name;
    int (*func)(str *arg, int index, int argc, char *argv[]);
    unsigned int has_arg:1, list_arg:1, long_opt:1;
};

int opt_arg_get();
int args_apply(struct option *options, int argc, char *argv[]);

#endif
