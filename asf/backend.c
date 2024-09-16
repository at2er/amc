#include "../include/backend.h"
#include "object.h"
#include "target.h"

extern struct object_array asm_objects;

int backend_init()
{
    objects_init();
    return 0;
}

int backend_file_new(struct file *f)
{
    object_append();
    return 0;
}

int backend_stop(enum BE_STOP_SIGNAL bess)
{
    switch (bess) {
    case BE_STOP_SIGNAL_NULL:
        //break;
    case BE_STOP_SIGNAL_ERR:
    default:
        objects_free();
        break;
    }

    return 0;
}

int backend_end()
{
    target_write(asm_objects.o[0]);
    return 0;
}

int backend_struct(struct file *f)
{
    return 0;
}

int backend_const(struct file *f)
{
    return 0;
}

int backend_var(struct file *f)
{
    return 0;
}

int backend_let(struct file *f)
{
    return 0;
}
