#ifndef sortTimsort_h
#define sortTimsort_h


#include <stddef.h>

#include "sort.h"
#include "sortInsertion.h"
#include "sortTimsortTemplate.h"


#define TIMSORT_RUN_SIZE 64


void timsort(
	void *const restrict array, const size_t arraySize,
	const size_t elementSize, compareFunc compare
);

timsortFlexibleDeclare(KeyValues, keyValue)


#endif