#ifndef sortInsertion_h
#define sortInsertion_h


#include <stddef.h>

#include "sort.h"
#include "sortInsertionTemplate.h"


void insertionSort(
	void *const restrict array, const size_t arraySize, const size_t elementSize,
	void *const restrict temp, compareFunc compare
);

insertionSortDeclare(KeyValues, keyValue)


#endif