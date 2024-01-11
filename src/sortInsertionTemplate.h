#ifndef sortInsertionTemplate_h
#define sortInsertionTemplate_h


/** This file should only be included by "sort.h"! **/


// This function macro allows us to declare
// prototypes for custom sorting functions.
#define insertionSortDeclare(name, type) \
	void insertionSort##name(type *const restrict array, const size_t arraySize);

// Assuming a function prototype has been created using
// the macro above, this macro provides the definition.
#define insertionSortDefine(name, type, compare)                                             \
	void insertionSort##name(type *const restrict array, const size_t arraySize){            \
		type *sort = array;                                                                  \
		const type *const last = &array[arraySize];                                          \
		for(; sort < last; ++sort){                                                          \
			const type temp = *sort;                                                         \
                                                                                             \
			type *check = sort - 1;                                                          \
			for(; check >= array && compare(check, &temp) == SORT_COMPARE_GREATER; --check){ \
				check[1] = *check;                                                           \
			}                                                                                \
			check[1] = temp;                                                                 \
		}                                                                                    \
	}


#endif