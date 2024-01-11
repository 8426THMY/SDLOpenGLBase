#ifndef sort_h
#define sort_h


#include <stddef.h>

#include "memoryManager.h"

#include "utilTypes.h"


#define SORT_COMPARE_LESSER -1
#define SORT_COMPARE_EQUAL   0
#define SORT_COMPARE_GREATER 1

#define TIMSORT_RUN_SIZE 64


/** All of the sorting functions defined in this file sort from lesser to greater. **/
/** We can always get the reverse ordering by negating the comparison function.    **/


// Return -1 if x < y, 0 if x = y and 1 if x > y.
// We use this for sorting objects whose ordering depends on a float.
//
// Note that these are prone to double evaluation,
// so the equivalent functions should be used to prevent this.
#define compareFloatFast(x, y) (((x) < (y)) ? SORT_COMPARE_LESSER : (((x) > (y)) ? SORT_COMPARE_GREATER : SORT_COMPARE_EQUAL))

// Specialized sorting function template macros.
// Useless by themselves, these files should only
// be included by this header file in particular.
#include "sortInsertionTemplate.h"
#include "sortTimsortTemplate.h"


typedef flags_t sort_t;

// For larger structures, it's often faster
// to sort an array of key-values instead.
typedef struct keyValue {
	// Value to sort by.
	float value;
	// Index of the represented object in its array.
	size_t key;
} keyValue;


sort_t compareFloat(const float x, const float y);
sort_t compareKeyValue(const keyValue *const restrict kv1, const keyValue *const restrict kv1);

// Key-value sorting functions.
timsortDeclare(KeyValues, keyValue)

// Generic sorting functions. If you want something more performant, use the function macros.
void insertionSort(
	void *const restrict array, const size_t arraySize, const size_t elementSize, void *const restrict temp,
	sort_t (*const compare)(const void *const restrict e1, const void *const restrict e2)
);
void timsort(
	void *const restrict array, const size_t arraySize, const size_t elementSize,
	sort_t (*const compare)(const void *const restrict e1, const void *const restrict e2)
);


#endif