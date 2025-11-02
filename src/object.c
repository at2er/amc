/* This file is part of amc.
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "flags.h"
#include "object.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

struct yz_object *create_yz_object(void)
{
	struct yz_object *self = calloc(1, sizeof(*self));
	if (amc_flags.debug)
		printf(DEBUG_FMT"object created\n");
	return self;
}

int end_object_life(struct yz_object *self)
{
	if (amc_flags.debug)
		printf(DEBUG_FMT"object life end\n");
	yz_object_listener_list_for_each(cur, self->after_life_end.begin) {
		if (!cur->notify)
			continue;
		if (cur->notify(cur->data))
			return 1;
	}
	return 0;
}

void free_yz_object(struct yz_object *self)
{
}

void listen_object_event(struct yz_object_listener_list *event,
		struct yz_object_listener *listener)
{
	assert(event && listener);
	listener->next = NULL;
	listener->prev = event->end;
	if (listener->prev) {
		listener->prev->next = listener;
	} else {
		event->begin = listener;
	}
	event->end = listener;
}

void refer_yz_object(struct yz_object *self)
{
}
