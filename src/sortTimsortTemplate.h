#ifndef sortTimsortTemplate_h
#define sortTimsortTemplate_h


/*
** These function macros allow us to declare
** prototypes for custom sorting functions.
*/

#define timsortDeclare(name, type)   \
	insertionSortDeclare(name, type) \
	void timsort##name(type *const restrict array, const size_t arraySize);

#define timsortFlexibleDeclare(name, type)                                                    \
	insertionSortFlexibleDeclare(name, type)                                                  \
	void timsort##name##Flexible(                                                             \
		type *const restrict array, const size_t arraySize,                                   \
		sort_t (*const compare)(const type *const restrict e1, const type *const restrict e2) \
	);

/*
** Assuming function prototypes have been created using
** the macros above, these macros provide the definitions.
*/

// Make sure to use "insertionSortDefine(name, type, compare)" beforehand!
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

// Make sure to use "insertionSortFlexibleDefine(name, type)" beforehand!
#define timsortFlexibleDefine(name, type)                                                            \
	static void mergeHalves##name##Flexible(                                                         \
		type *array, type *leftArray,                                                                \
		type *rightArray, const type *const restrict rightLast,                                      \
		sort_t (*const compare)(const type *const restrict e1, const type *const restrict e2)        \
	){                                                                                               \
                                                                                                     \
		const type *const leftLast = rightArray;                                                     \
                                                                                                     \
		for(;;){                                                                                     \
			if((*compare)(leftArray, rightArray) != SORT_COMPARE_GREATER){                           \
				*array = *leftArray;                                                                 \
				++leftArray;                                                                         \
                                                                                                     \
				if(leftArray >= leftLast){                                                           \
					do {                                                                             \
						++array;                                                                     \
						*array = *rightArray;                                                        \
						++rightArray;                                                                \
					} while(rightArray < rightLast);                                                 \
                                                                                                     \
					break;                                                                           \
				}                                                                                    \
			}else{                                                                                   \
				*array = *rightArray;                                                                \
				++rightArray;                                                                        \
                                                                                                     \
				if(rightArray >= rightLast){                                                         \
					do {                                                                             \
						++array;                                                                     \
						*array = *leftArray;                                                         \
						++leftArray;                                                                 \
					} while(leftArray < leftLast);                                                   \
                                                                                                     \
					break;                                                                           \
				}                                                                                    \
			}                                                                                        \
                                                                                                     \
			++array;                                                                                 \
		}                                                                                            \
	}                                                                                                \
                                                                                                     \
	void timsort##name##Flexible(                                                                    \
		type *const restrict array, const size_t arraySize,                                          \
		sort_t (*const compare)(const type *const restrict e1, const type *const restrict e2)        \
	){                                                                                               \
                                                                                                     \
		type *subArray = array;                                                                      \
		size_t size = TIMSORT_RUN_SIZE;                                                              \
                                                                                                     \
		type *const tempArray = memoryManagerGlobalAlloc(arraySize * sizeof(type));                  \
		if(tempArray == NULL){                                                                       \
			/** MALLOC FAILED **/                                                                    \
		}                                                                                            \
                                                                                                     \
		for(;;){                                                                                     \
			if(size < arraySize){                                                                    \
				insertionSortFlexible##name(subArray, TIMSORT_RUN_SIZE, compare);                    \
			}else{                                                                                   \
				insertionSortFlexible##name(subArray, TIMSORT_RUN_SIZE + arraySize - size, compare); \
				break;                                                                               \
			}                                                                                        \
			subArray += TIMSORT_RUN_SIZE;                                                            \
			size += TIMSORT_RUN_SIZE;                                                                \
		}                                                                                            \
                                                                                                     \
                                                                                                     \
		for(size = TIMSORT_RUN_SIZE; size < arraySize;){                                             \
			const size_t nextSize = 2 * size;                                                        \
			size_t left = 0;                                                                         \
                                                                                                     \
			memcpy(tempArray, array, arraySize * sizeof(type));                                      \
                                                                                                     \
			for(;;){                                                                                 \
				const size_t mid = left + size;                                                      \
				const size_t right = left + nextSize;                                                \
                                                                                                     \
				if(right < arraySize){                                                               \
					mergeHalvesFlexible##name(                                                       \
						&array[left], &tempArray[left],                                              \
						&tempArray[mid], &tempArray[right],                                          \
						compare                                                                      \
					);                                                                               \
				}else{                                                                               \
					if(mid < arraySize){                                                             \
						mergeHalvesFlexible##name(                                                   \
							&array[left], &tempArray[left],                                          \
							&tempArray[mid], &tempArray[arraySize],                                  \
							compare                                                                  \
						);                                                                           \
					}else{                                                                           \
						memcpy(&array[left], &tempArray[left], (arraySize - left) * sizeof(type));   \
					}                                                                                \
                                                                                                     \
					break;                                                                           \
				}                                                                                    \
                                                                                                     \
				left += nextSize;                                                                    \
			}                                                                                        \
                                                                                                     \
			size = nextSize;                                                                         \
		}                                                                                            \
                                                                                                     \
                                                                                                     \
		memoryManagerGlobalFree(tempArray);                                                          \
	}


#endif