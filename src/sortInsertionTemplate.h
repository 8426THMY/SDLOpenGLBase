#ifndef sortInsertionTemplate_h
#define sortInsertionTemplate_h


/*
** These function macros allow us to declare
** prototypes for custom sorting functions.
*/

#define insertionSortDeclare(name, type) \
	void insertionSort##name(type *const restrict array, const size_t arraySize);

#define insertionSortFlexibleDeclare(name, type)                                              \
	void insertionSort##name##Flexible(                                                       \
		type *const restrict array, const size_t arraySize,                                   \
		sort_t (*const compare)(const type *const restrict e1, const type *const restrict e2) \
	);

/*
** Assuming function prototypes have been created using
** the macros above, these macros provide the definitions.
*/

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

#define insertionSortFlexibleDefine(name, type)                                                 \
	void insertionSort##name##Flexible(                                                         \
		type *const restrict array, const size_t arraySize,                                     \
		sort_t (*const compare)(const type *const restrict e1, const type *const restrict e2)   \
	){                                                                                          \
                                                                                                \
		type *sort = array;                                                                     \
		const type *const last = &array[arraySize];                                             \
		for(; sort < last; ++sort){                                                             \
			const type temp = *sort;                                                            \
                                                                                                \
			type *check = sort - 1;                                                             \
			for(; check >= array && (*compare)(check, &temp) == SORT_COMPARE_GREATER; --check){ \
				check[1] = *check;                                                              \
			}                                                                                   \
			check[1] = temp;                                                                    \
		}                                                                                       \
	}


#endif