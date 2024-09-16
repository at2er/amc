#ifndef AMC_UNIT_H
#define AMC_UNIT_H
#include "../utils/str/str.h"
#include "file.h"
#include "type.h"

/**
 * @note:
 *   Won't automatically new a str.
 *   Won't automatically add \0 to end of str.
 */
int unit_read(str *unit, struct file *f);

/**
 * @note:
 *   Won't automatically new a str.
 *   Won't automatically add \0 to end of str.
 */
int unit_read_until(const char *es, str *unit, struct file *f);

/**
 * Basically the same as 'unit_read'.
 * But it will return the type attached to the unit.
 * @note:
 *   Don't use it to read ordinary unit.
 *   Won't automatically new a str.
 *   Won't automatically add \0 to end of str.
 */
enum ATOM_TYPE unit_and_type_read(str *unit, struct file *f);

#endif
