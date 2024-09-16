#include "object.h"
#include <stdlib.h>

struct object_array asm_objects = {NULL, 0 ,-1};

int object_append()
{
    asm_objects.cur++;
    asm_objects.len++;
    asm_objects.o = realloc(asm_objects.o, asm_objects.len * sizeof(struct object*));
    asm_objects.o[asm_objects.cur] = malloc(sizeof(struct object));
    object_section_init(&asm_objects.o[asm_objects.cur]->texts);
    object_section_init(&asm_objects.o[asm_objects.cur]->datas);
    object_section_init(&asm_objects.o[asm_objects.cur]->rodatas);
    return 0;
}

int object_section_append(struct section_head *head, struct section_node *node)
{
    if (head->last == NULL) {
        head->head = node;
        head->last = node;
        return 0;
    }

    node->prev = head->last;
    node->next = NULL;
    head->last->next = node;
    head->last = node;
    return 0;
}

int object_section_init(struct section_head *o)
{
    o->head = NULL;
    o->last = o->head;
    return 0;
}

int objects_init()
{
    asm_objects.o = malloc(sizeof(struct object *));
    return 0;
}

void object_free(struct object *o)
{
    object_section_free(&o->texts);
    object_section_free(&o->datas);
    object_section_free(&o->rodatas);
}

void object_section_free(struct section_head *head)
{
    struct section_node *c = head->head, *next;
    while (c->next != NULL) {
        next = c->next;
        str_free(c->s);
        free(c);
        c = next;
    }
}

void objects_free()
{
    if (asm_objects.o == NULL)
        return;
    for (int i = 0; i < asm_objects.len; i++) {
        object_free(asm_objects.o[i]);
    }
    free(asm_objects.o);
}
