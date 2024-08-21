#include "args.h"
#include "die.h"

static int args_long_option(struct option *opts, int index, int argc,
                            char *argv[]);
static int args_short_option(struct option *opts, int index, int argc,
                             char *argv[]);
static str *args_option_arg_read(struct option *opt, int index, int argc,
                                 char *argv[]);
static str *args_option_arg_oread(int index, int offset, int argc,
                                  char *argv[]);

int args_long_option(struct option *opts, int index, int argc, char *argv[])
{
    str *s = str_new();
    int opt = -1;

    for (int i = 2; argv[index][i] != '\0'; i++) {
        str_append(s, 1, &argv[index][i]);
    }

    for (int i = 0; opts[i].name != NULL && opts[i].func != NULL; i++) {
        if (opts[i].long_opt && strncmp(opts[i].name, s->s, s->len) == 0) {
            opt = i;
            break;
        }
    }
    if (opt == -1) {
        str_append(s, 1, "\0");
        die("amc: args_short_option: no such long option: %s\n", s->s);
    }

    str *arg = args_option_arg_read(&opts[opt], index, argc, argv);
    opts[opt].func(arg, index, argc, argv);
    if (opts[opt].has_arg)
        return 1;

    return 0;
}

int args_short_option(struct option *opts, int index, int argc, char *argv[])
{
    int opt = -1;
    for (int i = 0; opts[i].name != NULL && opts[i].func != NULL; i++) {
        if (opts[i].name[0] == argv[index][1]) {
            opt = i;
            break;
        }
    }
    if (opt == -1)
        die("amc: args_short_option: no such short option: %c\n",
            argv[index][1]);

    str *arg = args_option_arg_read(&opts[opt], index, argc, argv);
    opts[opt].func(arg, index, argc, argv);
    if (opts[opt].has_arg)
        return 1;

    return 0;
}

str *args_option_arg_read(struct option *opt, int index, int argc,
                          char *argv[])
{
    if (opt->has_arg != 1) {
        return NULL;
    } else if (!opt->long_opt) {
        if (argv[index][2] != '\0')
            return args_option_arg_oread(index, 2, argc, argv);
        return args_option_arg_oread(index + 1, 0, argc, argv);
    } else if (opt->long_opt) {
        return args_option_arg_oread(index + 1, 0, argc, argv);
    }

    return NULL;
}

str *args_option_arg_oread(int index, int offset, int argc, char *argv[])
{
    str *s = str_new();

    for (int i = offset; argv[index][i] != '\0'; i++) {
        str_append(s, 1, &argv[index][i]);
    }

    return s;
}

int args_apply(struct option *options, int argc, char *argv[])
{
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] != '-') {
            die("amc: args_apply: no such option\n");
        } else if (argv[i][1] == '-') {
            int offset = args_long_option(options, i, argc, argv);
            if (offset)
                i += offset;
            continue;
        }

        int offset = args_short_option(options, i, argc, argv);
        if (offset)
            i += offset;
    }

    return 0;
}
