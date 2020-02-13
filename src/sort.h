#ifndef sort_h
#define sort_h


#include <stddef.h>

#include "memoryManager.h"

#include "utilTypes.h"


#define SORT_COMPARE_LESSER -1
#define SORT_COMPARE_EQUAL   0
#define SORT_COMPARE_GREATER 1

#define TIMSORT_RUN_SIZE 64


// Return -1 if x is less than y, 0 if x is equal to y and 1 if x is greater than y.
// We generally use this for sorting objects whose ordering depends on a float.
//
// Note that these are prone to double evaluation,
// so the equivalent functions should be used to prevent this.
#define compareFloatFast(x, y) (((x) < (y)) ? SORT_COMPARE_LESSER : (((x) > (y)) ? SORT_COMPARE_GREATER : SORT_COMPARE_EQUAL))

// These function macros allow us to declare
// prototypes for custom sorting functions.
#define insertionSortDeclare(name, type) \
	void insertionSort##name(type *const restrict array, const size_t arraySize);
#define timsortDeclare(name, type)   \
	insertionSortDeclare(name, type) \
	void timsort##name(type *const restrict array, const size_t arraySize);

// Assuming a function prototype has been created using
// the macros above, these macros provide the definitions.
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
#define timsortDefine(name, type, compare)                                                                         \
	static void mergeHalves##name(                                                                                 \
		type *array, type *leftArray,                                                                              \
		type *rightArray, const type *const restrict rightLast                                                     \
	){                                                                                                             \
                                                                                                                   \
		const type *const leftLast = rightArray;                                                                   \
                                                                                                                   \
		for(;;){                                                                                                   \
			if(compare(leftArray, rightArray) != SORT_COMPARE_GREATER){                                            \
				*array = *leftArray;                                                                               \
				++leftArray;                                                                                       \
                                                                                                                   \
				if(leftArray >= leftLast){                                                                         \
					do {                                                                                           \
						++array;                                                                                   \
						*array = *rightArray;                                                                      \
						++rightArray;                                                                              \
					} while(rightArray < rightLast);                                                               \
                                                                                                                   \
					break;                                                                                         \
				}                                                                                                  \
			}else{                                                                                                 \
				*array = *rightArray;                                                                              \
				++rightArray;                                                                                      \
                                                                                                                   \
				if(rightArray >= rightLast){                                                                       \
					do {                                                                                           \
						++array;                                                                                   \
						*array = *leftArray;                                                                       \
						++leftArray;                                                                               \
					} while(leftArray < leftLast);                                                                 \
                                                                                                                   \
					break;                                                                                         \
				}                                                                                                  \
			}                                                                                                      \
                                                                                                                   \
			++array;                                                                                               \
		}                                                                                                          \
	}                                                                                                              \
                                                                                                                   \
	insertionSortDefine(name, type, compare)                                                                       \
                                                                                                                   \
	void timsort##name(type *const restrict array, const size_t arraySize){                                        \
		type *subArray = array;                                                                                    \
		size_t size = TIMSORT_RUN_SIZE;                                                                            \
                                                                                                                   \
		type *const tempArray = memoryManagerGlobalAlloc(arraySize * sizeof(type));                                \
		if(tempArray == NULL){                                                                                     \
			/** MALLOC FAILED **/                                                                                  \
		}                                                                                                          \
                                                                                                                   \
		for(;;){                                                                                                   \
			if(size < arraySize){                                                                                  \
				insertionSort##name(subArray, TIMSORT_RUN_SIZE);                                                   \
			}else{                                                                                                 \
				insertionSort##name(subArray, TIMSORT_RUN_SIZE + arraySize - size);                                \
				break;                                                                                             \
			}                                                                                                      \
			subArray += TIMSORT_RUN_SIZE;                                                                          \
			size += TIMSORT_RUN_SIZE;                                                                              \
		}                                                                                                          \
                                                                                                                   \
                                                                                                                   \
		for(size = TIMSORT_RUN_SIZE; size < arraySize;){                                                           \
			const size_t nextSize = 2 * size;                                                                      \
			size_t left = 0;                                                                                       \
                                                                                                                   \
			memcpy(tempArray, array, arraySize * sizeof(type));                                                    \
                                                                                                                   \
			for(;;){                                                                                               \
				const size_t mid = left + size;                                                                    \
				const size_t right = left + nextSize;                                                              \
                                                                                                                   \
				if(right < arraySize){                                                                             \
					mergeHalves##name(&array[left], &tempArray[left], &tempArray[mid], &tempArray[right]);         \
				}else{                                                                                             \
					if(mid < arraySize){                                                                           \
						mergeHalves##name(&array[left], &tempArray[left], &tempArray[mid], &tempArray[arraySize]); \
					}else{                                                                                         \
						memcpy(&array[left], &tempArray[left], (arraySize - left) * sizeof(type));                 \
					}                                                                                              \
                                                                                                                   \
					break;                                                                                         \
				}                                                                                                  \
                                                                                                                   \
				left += nextSize;                                                                                  \
			}                                                                                                      \
                                                                                                                   \
			size = nextSize;                                                                                       \
		}                                                                                                          \
                                                                                                                   \
                                                                                                                   \
		memoryManagerGlobalFree(tempArray);                                                                        \
	}


return_t compareFloat(const float x, const float y);

// Generic sorting functions. If you want something more performant, use the function macros.
void insertionSort(
	void *const restrict array, const size_t arraySize, const size_t elementSize, void *const restrict temp,
	return_t (*const compare)(const void *const restrict e1, const void *const restrict e2)
);
void timsort(
	void *const restrict array, const size_t arraySize, const size_t elementSize,
	return_t (*const compare)(const void *const restrict e1, const void *const restrict e2)
);


#endif