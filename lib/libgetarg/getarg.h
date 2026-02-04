/**
 * Please Comply with open source license.
 * @license: MIT License
 * @source: https://github.com/at2er/libgetarg
 */

#ifndef __LIBGETARG_H
#define __LIBGETARG_H

#ifndef __LIBGETARG_H__DEF_OPTION
#define __LIBGETARG_H__DEF_OPTION
struct option;
typedef int (*getarg_opt_parser)(int argc, char **argv, struct option *opt);

enum GETARG_HELP_OPT_PARSER_TYPE {
	GETARG_HELP_AFTER_USAGE,
	GETARG_HELP_ENDING
};

enum GETARG_OPT_TYPE {
	GETARG_HELP_OPT,
	GETARG_LIST_ARG,
	GETARG_NO_ARG,
	GETARG_SINGLE_ARG
};

struct option {
	const char *long_name;
	char short_name;
	enum GETARG_OPT_TYPE type;
	int is_optional_arg;
	getarg_opt_parser parse;

	/* optional */
	const char *document;
	const char *opt_doc;
};
#endif

#ifndef __LIBGETARG_H__DEF_FUNCTIONS
#define __LIBGETARG_H__DEF_FUNCTIONS
int getarg(int argc, char *argv[], struct option *opts);
void getarg_help_opt(struct option *opt, struct option *opts);
#endif

#endif
