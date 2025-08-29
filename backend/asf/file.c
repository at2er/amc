/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "include/asf.h"
#include "include/file.h"
#include "../../include/backend/target.h"
#include <stdlib.h>
#include <string.h>

int asf_file_end(const char *path, int path_len)
{
	struct object_head *prev = cur_obj->prev;
	if (target_write(path, path_len, cur_obj))
		return 1;
	if (backend_assemble_file(path, path_len))
		return 1;
	free_object_head(cur_obj);
	cur_obj = prev;
	return 0;
}

char *asf_file_get_suffix(int *result_len, int *need_free)
{
	*result_len = 2;
	*need_free = 0;
	return ".s";
}

int asf_file_new(void)
{
	const char *temp_rodata = ".section .rodata\n";
	struct object_node *rodata = NULL;
	struct object_head *prev = cur_obj;
	cur_obj = calloc(1, sizeof(*cur_obj));
	cur_obj->prev = prev;
	cur_obj->sec_count = 3;
	cur_obj->sections = calloc(cur_obj->sec_count,
			sizeof(*cur_obj->sections));
	rodata = malloc(sizeof(*rodata));
	if (object_append(&cur_obj->sections[ASF_OBJ_RODATA], rodata))
		goto err_free_rodata;
	rodata->s = str_new();
	str_append(rodata->s, strlen(temp_rodata), temp_rodata);
	return 0;
err_free_rodata:
	free(rodata);
	return 1;
}

int asf_link_files(str *output)
{
	return backend_link_files(output);
}
