#ifndef sortTimsortTemplate_h
#define sortTimsortTemplate_h


/** This file should only be included by "sort.h"! **/


// This function macro allows us to declare
// prototypes for custom sorting functions.
#define timsortDeclare(name, type)   \
	insertionSortDeclare(name, type) \
	void timsort##name(type *const restrict array, const size_t arraySize);

// Assuming a function prototype has been created using
// the macro above, this macro provides the definition.
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


#endif