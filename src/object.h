/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later

   Just for life cycle, not OOP's object.
*/
#ifndef AMC_OBJECT_H
#define AMC_OBJECT_H
#include <mcb/operand.h>
#include <stdint.h>

#define yz_object_listener_list_for_each(CUR, BEGIN) \
	for (struct yz_object_listener *CUR = BEGIN, \
			*__yz_obj_listener_list_for_each__next; \
			__yz_obj_listener_list_for_each__next = CUR \
			? CUR->next : NULL, \
			CUR != NULL; \
			CUR = __yz_obj_listener_list_for_each__next)

struct yz_object_listener {
	struct yz_object_listener *next, *prev;
	int (*notify)(void *data);
	void *data;
};

struct yz_object_listener_list {
	struct yz_object_listener *begin, *end;
};

struct yz_object {
	uint32_t ref_count;

	struct yz_object_listener_list after_life_end;
};

struct yz_object *create_yz_object(void);
int end_object_life(struct yz_object *self);
void free_yz_object(struct yz_object *self);
void listen_object_event(struct yz_object_listener_list *event,
		struct yz_object_listener *listener);
void refer_yz_object(struct yz_object *self);

#endif
