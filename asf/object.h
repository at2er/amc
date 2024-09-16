#ifndef AMC_BE_OBJECT_H
#define AMC_BE_OBJECT_H
#include "../parser/token.h"
#include "../utils/str/str.h"

struct section_node;
struct section_node {
    str *s;
    struct section_node *next;
    struct section_node *prev;
};

struct section_head {
    struct section_node *head;
    struct section_node *last;
};

struct object {
    struct section_head texts;
    struct section_head datas;
    struct section_head rodatas;
};

/**
 * @field o: The object must be dynamically allocated.
 */
struct object_array {
    struct object **o;
    unsigned int len;
    int cur;
};

int object_append();
int object_section_append(struct section_head *head, struct section_node *src);
int object_section_init(struct section_head *o);
int objects_init();
void object_free(struct object *o);
void object_section_free(struct section_head *head);
void objects_free();

#endif
