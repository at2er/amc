#include "unit.h"
#include "../utils/str/str.h"

int unit_read(str *unit, char* content)
{
    for (int i = 0; content[i] != ' '; i++) {
        if (content[i] == '\0') {
            printf("gmc: unit_read: end of file.\n");
            return 1;
        }

        str_append(unit, 1, &content[i]);
    }

    return 0;
}
