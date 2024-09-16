#include "register.h"
#include "../include/backend.h"

char mov_len_get(struct reg *reg)
{
    switch (reg->flags.bit_64) {
    case 0:
        return 'l';
        break;
    case 1:
        return 'q';
        break;
    default:
        printf("amc: backend_return: unsupport type");
        backend_stop(BE_STOP_SIGNAL_ERR);
        return 1;
        break;
    }

    return '\0';
}

struct reg *reg_get(enum ATOM_TYPE type)
{
    switch (atom_type_table[type - AM_TYPE_OFFSET].len) {
    case 4:
        break;
    case 8:
        break;
    default:
        printf("amc: reg_get: unsupport type");
        backend_stop(BE_STOP_SIGNAL_ERR);
        break;
    }

    return NULL;
}
