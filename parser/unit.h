#ifndef GMC_UNIT_H
#define GMC_UNIT_H
#include "../utils/str/str.h"
#include "file.h"
#include "type.h"

int unit_read(str *unit, struct file *f);
enum GM_TYPE unit_and_type_read(str *unit, struct file *f);

#endif
