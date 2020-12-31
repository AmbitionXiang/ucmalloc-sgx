#include "App.h"

void* sbrk_o(size_t size)
{
	void* result = NULL;
	result = sbrk((intptr_t)size);
	return result;
}

