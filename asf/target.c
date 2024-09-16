#include "target.h"
#include "../parser/parser.h"
#include "../utils/die.h"
#include <stdio.h>

static int write_section(const char *header, struct section_head *s, FILE *f);

int write_section(const char *header, struct section_head *s, FILE *f)
{
    struct section_node *c = s->head;

    fprintf(f, "%s\n", header);

    while (c != NULL) {
        str_append(c->s, 1, "\0");
        fputs(c->s->s, f);
        c = c->next;
    }

    return 0;
}

int target_write(struct object *t)
{
    if (parser_global_conf.has_err) {
        printf("amc: target_write: has error");
        return 1;
    }

    const char *name = "/tmp/test.out.s";
    FILE *fp = fopen(name, "w");

    if (fp == NULL)
        die("amc: target_write: cannot read or create file: %s", name);

    write_section(".text", &t->texts, fp);
    // write_section(".section .data", &t->datas, fp);
    // write_section(".section .rodata", &t->rodatas, fp);

    fclose(fp);

    return 0;
}
