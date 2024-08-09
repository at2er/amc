#include <stdio.h>

#define GMC_BACKEND_ASF
#define GMC_BACKEND_NATIVE
#define GMC_VERSION "0.0.0"

#if defined(GMC_BACKEND_ASF)
#include "asf/asf.h"
#elif defined(GMC_BACKEND_NATIVE)
#include "native/native.h"
#else
#error not have any backend
#endif

#include "parser/parser.h"

int main(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] != '-') {
            printf("Unknown option");
            return 1;
        }

        switch (argv[i][1]) {
        case 'v':
            printf("Germ Compiler v%s", GMC_VERSION);
            break;
        }
    }
}
