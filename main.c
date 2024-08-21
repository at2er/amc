#include "utils/args.h"
#include "utils/die.h"
#include <stdio.h>

#define ATOM_BACKEND_ASF
#define ATOM_BACKEND_NATIVE
#define ATOM_VERSION "0.0.0"

static int print_version(str *arg, int index, int argc, char *argv[]);
static int source_comp(str *arg, int index, int argc, char *argv[]);

#if defined(ATOM_BACKEND_ASF)
#include "asf/asf.h"
#elif defined(GMC_BACKEND_NATIVE)
#else
#error not have any backend
#endif

#include "parser/file.h"
#include "parser/keywords.h"
#include "parser/parser.h"
#include "parser/token.h"

int print_version(str *arg, int index, int argc, char *argv[])
{
    die("Atom Compiler v%s\n", ATOM_VERSION);
    return 0;
}

int source_comp(str *arg, int index, int argc, char *argv[])
{
    struct file f;
    str_append(arg, 1, "\0");
    parser_init(arg->s, &f);
    return 0;
}

int main(int argc, char *argv[])
{
    struct option options[] = {
        {"v",  print_version, 0, 0, 0},
        {"s",  source_comp,   1, 1, 0},
        //{NULL, source_comp,   0, 0, 0},
        {NULL, NULL,          0, 0, 0}
    };

    token_groups_init();
    keyword_init();
    args_apply(options, argc, argv);
}
