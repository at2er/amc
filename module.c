#include "module.h"
#include <memory.h>

void yz_module_init(struct yz_module *self)
{
	memset(self->symbols, '\0', sizeof(*self->symbols));
}
