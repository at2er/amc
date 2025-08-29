/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef AMC_BACKEND_H
#define AMC_BACKEND_H
#include "backend/array.h"
#include "backend/cond.h"
#include "backend/const.h"
#include "backend/decorator.h"
#include "backend/expr.h"
#include "backend/func.h"
#include "backend/loop.h"
#include "backend/null.h"
#include "backend/operator.h"
#include "backend/ptr.h"
#include "backend/scope.h"
#include "backend/struct.h"
#include "backend/symbol.h"

#define backend_call(FUNC) backends[cur_backend]->FUNC

enum BACKENDS {
	BE_NONE,
	BE_ASF
};

enum BE_FLAG {
	BE_FLAG_INITED = 1 << 0,
	BE_FLAG_STOPED = 1 << 1,
	BE_FLAG_ENDED  = 1 << 2,
};

enum BE_STOP_SIGNAL {
	BE_STOP_SIGNAL_ERR,  // stop backend and notify backend has errors
	BE_STOP_SIGNAL_NULL, // no reason stop backend
	BE_STOP_SIGNAL_NE,   // no problem then stop backend
};

/**
 * Backend interface.
 * @note: control signal declaration in this file.
 */
struct backend {
	int (*end)(str *output);
	int (*file_end)(const char *path, int path_len);
	char *(*file_get_suffix)(int *result_len, int *need_free);
	int (*file_new)(void);
	int (*init)(int argc, char *argv[]);
	int (*stop)(enum BE_STOP_SIGNAL bess);
	backend_array_def_f                array_def;
	backend_array_set_elem_f           array_set_elem;
	backend_cond_elif_f                cond_elif;
	backend_cond_else_f                cond_else;
	backend_cond_if_f                  cond_if;
	backend_cond_if_begin_f            cond_if_begin;
	backend_cond_if_cond_f             cond_if_cond;
	backend_cond_if_end_f              cond_if_end;
	backend_cond_if_free_handle_f      cond_if_free_handle;
	backend_cond_match_begin_f         cond_match_begin;
	backend_cond_match_case_f          cond_match_case;
	backend_cond_match_case_end_f      cond_match_case_end;
	backend_cond_match_end_f           cond_match_end;
	backend_cond_match_free_handle_f   cond_match_free_handle;
	backend_const_def_str_f            const_def_str;
	backend_const_free_data_f          const_free_data;
	backend_dec_c_fn_f                 dec_c_fn;
	backend_dec_syscall_f              dec_syscall;
	backend_func_call_f                func_call;
	backend_func_def_f                 func_def;
	backend_func_def_end_f             func_def_end;
	backend_func_ret_f                 func_ret;
	backend_null_handle_begin_f        null_handle_begin;
	backend_null_handle_end_f          null_handle_end;
	backend_op_cmd_f                   ops[OP_LEN];
	backend_ptr_set_val_f              ptr_set_val;
	backend_scope_begin_f              scope_begin;
	backend_scope_end_f                scope_end;
	backend_scope_free_f               scope_free;
	backend_struct_def_f               struct_def;
	backend_struct_set_elem_f          struct_set_elem;
	backend_struct_set_elem_from_ptr_f struct_set_elem_from_ptr;
	backend_symbol_get_path_f          symbol_get_path;
	backend_symbol_status_free_f       symbol_status_free;
	backend_var_set_f                  var_set;
	backend_var_immut_init_f           var_immut_init;
	backend_while_begin_f              while_begin;
	backend_while_end_f                while_end;
	backend_while_cond_f               while_cond;
	backend_while_free_handle_f        while_free_handle;
};

extern char *backend_assembler;
extern char *backend_linker;
extern struct backend *backends[];
extern enum BACKENDS cur_backend;

int backend_append_assembler_flags(char *arg);
int backend_append_lib(char *path);
int backend_append_linker_flags(char *arg);

/**
 * Backend control functions.
 * @note: Backend control functions will block compiler operation.
 *        Don't do too time-consuming things.
 * @important: All control functions must be implemented.
 */

int backend_assemble_file(const char *path, int path_len);

/**
 * Compiler completed, notify backend operation.
 * End backend normally.
 */
int backend_end(str *output);

int backend_file_end(const char *target_path, int len);
char *backend_file_get_suffix(int *result_len, int *need_free);
int backend_file_new(void);

/**
 * Backend init.
 * @note: Arguments parse in selected backend.
 */
int backend_init(int argc, char *argv[]);

int backend_link_files(str *output);

/**
 * Notify the backend to stop compiling.
 * @important: All stop signals must be implemented.
 */
int backend_stop(enum BE_STOP_SIGNAL bess);

#endif
