#include "sortTimsort.h"


#define nextElement(array, elementSize)   ((void *)(((byte_t *)(array)) + (elementSize)))
#define prevElement(array, elementSize)   ((void *)(((byte_t *)(array)) - (elementSize)))
#define getElement(array, i, elementSize) ((void *)(((byte_t *)(array)) + (i) * (elementSize)))


#include <string.h>

#include "memoryManager.h"


// Forward-declare any helper functions!
static void mergeHalves(
	void *const restrict array, const size_t elementSize,
	void *const restrict leftArray, void *const restrict rightArray, const void *const restrict rightLast,
	compareFunc compare
);


/*
** Timsort is a good generic sorting algorithm,
** as it is stable, adaptive and very performant.
** In most cases, you'll want to use this.
*/
void timsort(
	void *const restrict array, const size_t arraySize,
	const size_t elementSize, compareFunc compare
){

	void *subArray = array;
	size_t size = TIMSORT_RUN_SIZE;

	// Allocate a temporary array to store a copy of our data when merging.
	void *const tempArray = memoryManagerGlobalAlloc(arraySize * elementSize);
	if(tempArray == NULL){
		/** MALLOC FAILED **/
	}


	// Sort the subarrays of the run size using insertion sort.
	for(;;){
		// If we haven't gone past the end of the array, sort the entire run!
		if(size < arraySize){
			insertionSort(subArray, TIMSORT_RUN_SIZE, elementSize, tempArray, compare);

		// Otherwise, sort the remaining elements!
		}else{
			insertionSort(subArray, TIMSORT_RUN_SIZE + arraySize - size, elementSize, tempArray, compare);
			break;
		}

		subArray = getElement(subArray, TIMSORT_RUN_SIZE, elementSize);
		size += TIMSORT_RUN_SIZE;
	}


	// Start merging the runs using merge sort!
	for(size = TIMSORT_RUN_SIZE; size < arraySize;){
		const size_t nextSize = 2 * size;
		size_t left = 0;

		// Copy the array's current state into the temporary array.
		memcpy(tempArray, array, arraySize * elementSize);

		// Merge every pair of subarrays of size blockSize.
		for(;;){
			const size_t mid = left + size;
			const size_t right = left + nextSize;

			// If we haven't gone past the end of the array, sort up to right!
			if(right < arraySize){
				mergeHalves(
					getElement(array, left, elementSize), elementSize,
					getElement(tempArray, left, elementSize),
					getElement(tempArray, mid, elementSize),
					getElement(tempArray, right, elementSize),
					compare
				);
			}else{
				// If we've gone past the end of the array but there are still some
				// elements in the right block, merge up to the end of the array.
				if(mid < arraySize){
					mergeHalves(
						getElement(array, left, elementSize), elementSize,
						getElement(tempArray, left, elementSize),
						getElement(tempArray, mid, elementSize),
						getElement(tempArray, arraySize, elementSize),
						compare
					);

				// Otherwise, we can just copy the left array since it's already sorted.
				}else{
					memcpy(
						getElement(array, left, elementSize),
						getElement(tempArray, left, elementSize),
						(arraySize - left) * elementSize
					);
				}

				break;
			}

			left += nextSize;
		}

		size = nextSize;
	}


	memoryManagerGlobalFree(tempArray);
}


/*
** Timsort arrays of key-values using the macros defined
** in "sortInsertionTemplate.h" and "sortTimsortTemplate.h".
*/
timsortFlexibleDefine(KeyValues, keyValue)


/*
** Assuming leftArray and rightArray are adjacent subarrays
** of array, merge them and store the result in array.
*/
static void mergeHalves(
	void *array, const size_t elementSize,
	void *leftArray, void *rightArray, const void *const restrict rightLast,
	compareFunc compare
){
	const void *const leftLast = rightArray;

	// Merge the two subarrays using a sliding window.
	for(;;){
		// If the element in the left array is lesser, add it to the main array.
		if((*compare)(leftArray, rightArray) != SORT_COMPARE_GREATER){
			memcpy(array, leftArray, elementSize);
			leftArray = nextElement(leftArray, elementSize);

			// If this was the last element in the left array,
			// copy the remaining elements from the right array.
			if(leftArray >= leftLast){
				do {
					array = nextElement(array, elementSize);
					memcpy(array, rightArray, elementSize);
					rightArray = nextElement(rightArray, elementSize);
				} while(rightArray < rightLast);

				break;
			}

		// Otherwise, add the element in the right array to the main array.
		}else{
			memcpy(array, rightArray, elementSize);
			rightArray = nextElement(rightArray, elementSize);

			// If this was the last element in the right array,
			// copy the remaining elements from the left array.
			if(rightArray >= rightLast){
				do {
					array = nextElement(array, elementSize);
					memcpy(array, leftArray, elementSize);
					leftArray = nextElement(leftArray, elementSize);
				} while(leftArray < leftLast);

				break;
			}
		}

		array = nextElement(array, elementSize);
	}
}