/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "../include/backend.h"
#include "../include/config.h"
#include "../utils/die.h"
#include <stdlib.h>
#include <string.h>

#if defined(__unix__)
#include <sys/wait.h>
#include <unistd.h>
#endif

extern struct backend backend_asf;

struct backend *backends[] = {
	NULL,
	&backend_asf
};

int backend_flag = 0;
enum BACKENDS cur_backend;

char **backend_assembled_files = NULL;
int backend_assembled_files_count = 0;
char *backend_assembler = "as";
char *backend_linker = "ld";

static char **assembler_flags = NULL;
static int assembler_flags_count = 0;
static char **libs_list = NULL;
static int libs_count = 0;
static char **linker_flags = NULL;
static int linker_flags_count = 0;

static int append_flag(char ***container, int *count, char **arg);
static int assemble_file_call_as(const char *path, int path_len, char *output);
static int link_files_call_linker(str *output);

int append_flag(char ***container, int *count, char **arg)
{
	int end = 0, len;
	char *tok = *arg;
	for (len = 0; tok[len] != ' '; len++) {
		if (tok[len] == '\0') {
			end = 1;
			break;
		}
	}
	*arg = &tok[len + 1];
	*count += 1;
	*container = realloc(*container,
			sizeof(**container) * *count);
	(*container)[*count - 1] = str2chr(tok, len);
	return end ? -1 : 0;
}

int assemble_file_call_as(const char *path, int path_len, char *output)
{
	char **argv;
	int argv_offset = 4;
	argv = calloc(1 + argv_offset
			+ assembler_flags_count,
			sizeof(*argv));
	close(STDIN_FILENO);
	argv[0] = backend_assembler;
	argv[1] = malloc(path_len);
	argv[2] = "-o";
	argv[3] = output;
	for (int i = 0; i < assembler_flags_count; i++, argv_offset++)
		argv[argv_offset] = assembler_flags[i];
	strncpy(argv[1], path, path_len);
	execvp(backend_assembler, argv);
	die("amc[\x1b[30;41mDIE\x1b[0m]: Assembler stopped!\n");
	return 1;
}

int link_files_call_linker(str *output)
{
	char **argv;
	int argv_offset = 3;
	argv = calloc(1 + argv_offset
			+ backend_assembled_files_count
			+ libs_count * 2,
			sizeof(*argv));
	argv[0] = backend_linker;
	argv[1] = "-o";
	argv[2] = output->s;
	for (int i = 0; i < backend_assembled_files_count; i++, argv_offset++)
		argv[argv_offset] = backend_assembled_files[i];
	for (int i = 0; i < linker_flags_count; i++, argv_offset++)
		argv[argv_offset] = linker_flags[i];
	for (int i = 0; i < libs_count; i++, argv_offset += 2) {
		argv[argv_offset] = "-l";
		argv[argv_offset + 1] = libs_list[i];
	}
	argv[argv_offset] = NULL;
	execvp(backend_linker, argv);
	die("amc[\x1b[30;41mDIE\x1b[0m]: Linker stopped!\n");
	return 0;
}

int backend_append_assembler_flags(char *arg)
{
	int ret = 0;
	while ((ret = append_flag(&assembler_flags,
					&assembler_flags_count, &arg)) != -1) {
		if (ret > 0)
			return 1;
	}
	return 0;
}

int backend_append_lib(char *path)
{
	libs_count += 1;
	libs_list = realloc(libs_list, sizeof(*libs_list) * libs_count);
	libs_list[libs_count - 1] = path;
	return 0;
}

int backend_append_linker_flags(char *arg)
{
	int ret = 0;
	while ((ret = append_flag(&linker_flags,
					&linker_flags_count, &arg)) != -1) {
		if (ret > 0)
			return 1;
	}
	return 0;
}

int backend_assemble_file(const char *path, int path_len)
{
	char *output = NULL;
	int ret = 0;
	output = malloc(path_len);
	strncpy(output, path, path_len);
	output[path_len - 2] = 'o';
	if ((ret = fork()) < 0)
		die("amc[\x1b[30;41mDIE\x1b[0m]: Assembler fork failed!\n");
	if (ret == 0)
		return assemble_file_call_as(path, path_len, output);
	wait(NULL);
	backend_assembled_files_count += 1;
	backend_assembled_files = realloc(backend_assembled_files,
			sizeof(*backend_assembled_files)
			* backend_assembled_files_count);
	backend_assembled_files[backend_assembled_files_count - 1] = output;
	return 0;
}

int backend_end(str *output)
{
	if (backend_flag & BE_FLAG_ENDED)
		return 0;
	if (backends[cur_backend]->end(output))
		return 1;
	for (int i = 0; i < backend_assembled_files_count; i++)
		free(backend_assembled_files[i]);
	free(backend_assembled_files);
	return 0;
}

int backend_file_end(const char *target_path, int len)
{
	if (backend_flag & BE_FLAG_STOPED)
		return 0;
	return backends[cur_backend]->file_end(target_path, len);
}

char *backend_file_get_suffix(int *result_len, int *need_free)
{
	return backends[cur_backend]->file_get_suffix(result_len, need_free);
}

int backend_file_new(void)
{
	return backends[cur_backend]->file_new();
}

int backend_init(int argc, char *argv[])
{
	if (backend_flag & BE_FLAG_INITED)
		return 0;
	if (cur_backend == BE_NONE)
		cur_backend = default_backend;
	return backends[cur_backend]->init(argc, argv);
}

int backend_link_files(str *output)
{
	int ret = 0;
	if ((ret = fork()) < 0)
		die("amc[\x1b[30;41mDIE\x1b[0m]: Linker fork failed!\n");
	if (ret == 0)
		return link_files_call_linker(output);
	wait(NULL);
	return 0;
}

int backend_stop(enum BE_STOP_SIGNAL bess)
{
	if (backend_flag & BE_FLAG_STOPED)
		return 0;
	if (backends[cur_backend]->stop(bess))
		return 1;
	backend_flag |= BE_FLAG_STOPED;
	return 0;
}

void free_backend_const_noself(backend_const *self)
{
	if (self == NULL)
		return;
	free_yz_val_noself(&self->val);
	if (self->data == NULL)
		return;
	backend_call(const_free_data)(self->data);
}
