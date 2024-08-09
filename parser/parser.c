#include "parser.h"
#include "file.h"

int parser_init(const char* path, struct file* f)
{
    file_init(path, f);
    return 0;
}
