#ifndef sort_h
#define sort_h


#include "utilTypes.h"


#define SORT_COMPARE_LESSER -1
#define SORT_COMPARE_EQUAL   0
#define SORT_COMPARE_GREATER 1


/** All of the sorting functions defined in this file sort from lesser to greater. **/
/** We can always get the reverse ordering by negating the comparison function.    **/


// Return -1 if x < y, 0 if x = y and 1 if x > y.
// We use this for sorting objects whose ordering depends on a float.
//
// Note that these are prone to double evaluation,
// so the equivalent functions should be used to prevent this.
#define compareFloatFast(x, y)                                      \
	(                                                               \
		((x) < (y)) ? SORT_COMPARE_LESSER : (                       \
			((x) > (y)) ? SORT_COMPARE_GREATER : SORT_COMPARE_EQUAL \
		)                                                           \
	)


typedef flags_t sort_t;

// For larger structures, it's often faster
// to sort an array of key-values instead.
typedef struct keyValue {
	// Value to sort by.
	float value;
	// Pointer to the represented object.
	void *key;
} keyValue;


sort_t compareFloat(const float x, const float y);
sort_t compareFloatReversed(const float x, const float y);
sort_t compareKeyValue(const keyValue *const restrict kv1, const keyValue *const restrict kv2);
sort_t compareKeyValueReversed(const keyValue *const restrict kv1, const keyValue *const restrict kv2);


#endif