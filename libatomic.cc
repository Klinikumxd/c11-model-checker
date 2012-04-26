#include "libatomic.h"
#include "model.h"
#include "common.h"

void atomic_store_explicit(struct atomic_object *obj, int value, memory_order order)
{
	DBG();
	model->switch_to_master(new ModelAction(ATOMIC_WRITE, order, obj, value));
	obj->value = value;
}

int atomic_load_explicit(struct atomic_object *obj, memory_order order)
{
	DBG();
	model->switch_to_master(new ModelAction(ATOMIC_READ, order, obj, VALUE_NONE));
	return obj->value;
}
