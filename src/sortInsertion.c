#include "sortInsertion.h"


#define nextElement(array, elementSize)   ((void *)(((byte_t *)(array)) + (elementSize)))
#define prevElement(array, elementSize)   ((void *)(((byte_t *)(array)) - (elementSize)))
#define getElement(array, i, elementSize) ((void *)(((byte_t *)(array)) + (i) * (elementSize)))


#include <string.h>

#include "memoryManager.h"


/*
** Insertion sort works best for small or mostly-sorted arrays.
** For larger arrays, it may be better to use a binary insertion sort.
**
** Note that temp must be large enough to contain at least elementSize bytes.
*/
void insertionSort(
	void *const restrict array, const size_t arraySize, const size_t elementSize,
	void *const restrict temp, compareFunc compare
){

	void *sort = nextElement(array, elementSize);
	const void *const last = getElement(array, arraySize, elementSize);

	// Sort every element in the array starting from the second.
	for(; sort < last; sort = nextElement(sort, elementSize)){
		void *check = prevElement(sort, elementSize);

		memcpy(temp, sort, elementSize);
		// Move the element we're sorting backwards through the
		// array until we find the element it should precede.
		for(; check >= array && (*compare)(check, temp) == SORT_COMPARE_GREATER; check = prevElement(check, elementSize)){
			// Shift this element over to the right,
			// as the one being sorted goes before it.
			memcpy(nextElement(check, elementSize), check, elementSize);
		}
		// Copy the current element into its correct place!
		memcpy(nextElement(check, elementSize), temp, elementSize);
	}
}


// Timsort arrays of key-values using the macro defined in "sortInsertionTemplate.h".
insertionSortDefine(KeyValues, keyValue, compareKeyValue)