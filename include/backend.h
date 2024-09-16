#ifndef AMC_BACKEND_H
#define AMC_BACKEND_H
#include "../parser/file.h"
#include "../parser/token.h"
#include "../parser/type.h"

enum BE_STOP_SIGNAL {
    BE_STOP_SIGNAL_ERR,  // stop backend and notify backend has errors
    BE_STOP_SIGNAL_NULL, // no reason stop backend
    BE_STOP_SIGNAL_NE,   // no problem then stop backend
};

/**
 * Backend control functions.
 * @note: Backend control functions will block compiler operation.
 *        Don't do too time-consuming things.
 * @important: All control functions must be implemented.
 */

/**
 */
int backend_init();

int backend_file_new(struct file *f);

/**
 * Notify the backend to stop compiling.
 * @important: All stop signals must be implemented.
 */
int backend_stop(enum BE_STOP_SIGNAL bess);

/**
 * Compiler completed, notify backend operation.
 * End backend normally.
 */
int backend_end();

int backend_for(struct file *f);
int backend_struct(struct file *f);
int backend_const(struct file *f);
int backend_var(struct file *f);
int backend_let(struct file *f);
int backend_if(struct file *f);
int backend_elif(struct file *f);
int backend_else(struct file *f);
int backend_match(struct file *f);
int backend_while(struct file *f);

int backend_cmd_add(struct file *f);
int backend_cmd_sub(struct file *f);
int backend_cmd_div(struct file *f);
int backend_cmd_mul(struct file *f);

#endif
