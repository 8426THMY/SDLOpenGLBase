#ifndef moduleShared_h
#define moduleShared_h


#include "memoryPool.h"
#include "memorySingleList.h"
#include "memoryDoubleList.h"


#warning "Is there a better way to handle the free functions?"


/*
** These function macros allow us to declare
** prototypes for custom module functions.
*/

// Pool allocators.
#define moduleDeclarePool(name, type, manager)             \
	type *module##name##Alloc();                           \
	void module##name##Free(type *const restrict element); \
	void module##name##Clear();                            \
	extern memoryPool manager;

// Single list allocators.
#define moduleDeclareSingleList(name, type, manager)                                                               \
	type *module##name##Alloc();                                                                                   \
	type *module##name##Prepend(type **const restrict start);                                                      \
	type *module##name##Append(type **const restrict start);                                                       \
	type *module##name##InsertAfter(type **const restrict start, type *const restrict prev);                       \
	type *module##name##Next(const type *const restrict element);                                                  \
	void module##name##Free(type **const restrict start, type *const restrict element, type *const restrict prev); \
	void module##name##FreeArray(type **const restrict start);                                                     \
	void module##name##Clear();                                                                                    \
	extern memorySingleList manager;

// Double list allocators.
#define moduleDeclareDoubleList(name, type, manager)                                          \
	type *module##name##Alloc();                                                              \
	type *module##name##Prepend(type **const restrict start);                                 \
	type *module##name##Append(type **const restrict start);                                  \
	type *module##name##InsertBefore(type **const restrict start, type *const restrict next); \
	type *module##name##InsertAfter(type **const restrict start, type *const restrict prev);  \
	type *module##name##Prev(const type *const restrict element);                             \
	type *module##name##Next(const type *const restrict element);                             \
	void module##name##Free(type **const restrict start, type *const restrict element);       \
	void module##name##FreeArray(type **const restrict start);                                \
	void module##name##Clear();                                                               \
	extern memoryDoubleList manager;

/*
** Assuming function prototypes have been created using
** the macros above, these macros provide the definitions.
*/

// Pool allocators.
#ifndef MEMORYREGION_EXTEND_ALLOCATORS
#define moduleDefinePool(name, type, manager, freeFunc, size)          \
	memoryPool manager;                                                \
                                                                       \
	type *module##name##Alloc(){                                       \
		return(memPoolAlloc(&manager));                                \
	}                                                                  \
                                                                       \
	void module##name##Free(type *const restrict element){             \
		freeFunc(element);                                             \
		memPoolFree(&manager, element);                                \
	}                                                                  \
                                                                       \
	void module##name##Clear(){                                        \
		MEMPOOL_LOOP_BEGIN(manager, i, type)                           \
			module##name##Free(i);                                     \
		MEMPOOL_LOOP_END(manager, i)                                   \
		memPoolClear(&manager);                                        \
	}
#else
#define moduleDefinePool(name, type, manager, freeFunc, size)          \
	memoryPool manager;                                                \
                                                                       \
	type *module##name##Alloc(){                                       \
		type *newBlock = memPoolAlloc(&manager);                       \
		if(newBlock == NULL){                                          \
			if(memPoolExtend(                                          \
				&manager,                                              \
				memoryManagerGlobalAlloc(memoryGetRequiredSize(size)), \
				memoryGetRequiredSize(size)                            \
			)){                                                        \
				newBlock = memPoolAlloc(&manager);                     \
			}                                                          \
		}                                                              \
		return(newBlock);                                              \
	}                                                                  \
                                                                       \
	void module##name##Free(type *const restrict element){             \
		freeFunc(element);                                             \
		memPoolFree(&manager, element);                                \
	}                                                                  \
                                                                       \
	void module##name##Clear(){                                        \
		MEMPOOL_LOOP_BEGIN(manager, i, type)                           \
			module##name##Free(i);                                     \
		MEMPOOL_LOOP_END(manager, i)                                   \
		memPoolClear(&manager);                                        \
	}
#endif

// Single list allocators.
#ifndef MEMORYREGION_EXTEND_ALLOCATORS
#define moduleDefineSingleList(name, type, manager, freeFunc, size)                                                \
	memorySingleList manager;                                                                                      \
                                                                                                                   \
	type *module##name##Alloc(){                                                                                   \
		return(memSingleListAlloc(&manager));                                                                      \
	}                                                                                                              \
                                                                                                                   \
	type *module##name##Prepend(type **const restrict start){                                                      \
		return(memSingleListPrepend(&manager, (void **)start));                                                    \
	}                                                                                                              \
                                                                                                                   \
	type *module##name##Append(type **const restrict start){                                                       \
		return(memSingleListAppend(&manager, (void **)start));                                                     \
	}                                                                                                              \
                                                                                                                   \
	type *module##name##InsertAfter(type **const restrict start, type *const restrict prev){                       \
		return(memSingleListInsertAfter(&manager, (void **)start, (void *)prev));                                  \
	}                                                                                                              \
                                                                                                                   \
	type *module##name##Next(const type *const restrict element){                                                  \
		return(memSingleListNext(element));                                                                        \
	}                                                                                                              \
                                                                                                                   \
	void module##name##Free(type **const restrict start, type *const restrict element, type *const restrict prev){ \
		freeFunc(element);                                                                                         \
		memSingleListFree(&manager, (void **)start, (void *)element, (void *)prev);                                \
	}                                                                                                              \
                                                                                                                   \
	void module##name##FreeArray(type **const restrict start){                                                     \
		type *element = *start;                                                                                    \
		while(element != NULL){                                                                                    \
			module##name##Free(start, element, NULL);                                                              \
			element = *start;                                                                                      \
		}                                                                                                          \
	}                                                                                                              \
                                                                                                                   \
	void module##name##Clear(){                                                                                    \
		MEMSINGLELIST_LOOP_BEGIN(manager, i, type)                                                                 \
			module##name##Free(NULL, i, NULL);                                                                     \
		MEMSINGLELIST_LOOP_END(manager, i)                                                                         \
		memSingleListClear(&manager);                                                                              \
	}
#else
#define moduleDefineSingleList(name, type, manager, freeFunc, size)                                                \
	memorySingleList manager;                                                                                      \
                                                                                                                   \
	type *module##name##Alloc(){                                                                                   \
		type *newBlock = memSingleListAlloc(&manager);                                                             \
		if(newBlock == NULL){                                                                                      \
			if(memSingleListExtend(                                                                                \
				&manager,                                                                                          \
				memoryManagerGlobalAlloc(memoryGetRequiredSize(size)),                                             \
				memoryGetRequiredSize(size)                                                                        \
			)){                                                                                                    \
				newBlock = memSingleListAlloc(&manager);                                                           \
			}                                                                                                      \
		}                                                                                                          \
		return(newBlock);                                                                                          \
	}                                                                                                              \
                                                                                                                   \
	type *module##name##Prepend(type **const restrict start){                                                      \
		type *newBlock = memSingleListPrepend(&manager, (void **)start);                                           \
		if(newBlock == NULL){                                                                                      \
			if(memSingleListExtend(                                                                                \
				&manager,                                                                                          \
				memoryManagerGlobalAlloc(memoryGetRequiredSize(size)),                                             \
				memoryGetRequiredSize(size)                                                                        \
			)){                                                                                                    \
				newBlock = memSingleListPrepend(&manager, (void **)start);                                         \
			}                                                                                                      \
		}                                                                                                          \
		return(newBlock);                                                                                          \
	}                                                                                                              \
                                                                                                                   \
	type *module##name##Append(type **const restrict start){                                                       \
		type *newBlock = memSingleListAppend(&manager, (void **)start);                                            \
		if(newBlock == NULL){                                                                                      \
			if(memSingleListExtend(                                                                                \
				&manager,                                                                                          \
				memoryManagerGlobalAlloc(memoryGetRequiredSize(size)),                                             \
				memoryGetRequiredSize(size)                                                                        \
			)){                                                                                                    \
				newBlock = memSingleListAppend(&manager, (void **)start);                                          \
			}                                                                                                      \
		}                                                                                                          \
		return(newBlock);                                                                                          \
	}                                                                                                              \
                                                                                                                   \
	type *module##name##InsertAfter(type **const restrict start, type *const restrict prev){                       \
		type *newBlock = memSingleListInsertAfter(&manager, (void **)start, (void *)prev);                         \
		if(newBlock == NULL){                                                                                      \
			if(memSingleListExtend(                                                                                \
				&manager,                                                                                          \
				memoryManagerGlobalAlloc(memoryGetRequiredSize(size)),                                             \
				memoryGetRequiredSize(size)                                                                        \
			)){                                                                                                    \
				newBlock = memSingleListInsertAfter(&manager, (void **)start, (void *)prev);                       \
			}                                                                                                      \
		}                                                                                                          \
		return(newBlock);                                                                                          \
	}                                                                                                              \
                                                                                                                   \
	type *module##name##Next(const type *const restrict element){                                                  \
		return(memSingleListNext(element));                                                                        \
	}                                                                                                              \
                                                                                                                   \
	void module##name##Free(type **const restrict start, type *const restrict element, type *const restrict prev){ \
		freeFunc(element);                                                                                         \
		memSingleListFree(&manager, (void **)start, (void *)element, (void *)prev);                                \
	}                                                                                                              \
                                                                                                                   \
	void module##name##FreeArray(type **const restrict start){                                                     \
		type *element = *start;                                                                                    \
		while(element != NULL){                                                                                    \
			module##name##Free(start, element, NULL);                                                              \
			element = *start;                                                                                      \
		}                                                                                                          \
	}                                                                                                              \
                                                                                                                   \
	void module##name##Clear(){                                                                                    \
		MEMSINGLELIST_LOOP_BEGIN(manager, i, type)                                                                 \
			module##name##Free(NULL, i, NULL);                                                                     \
		MEMSINGLELIST_LOOP_END(manager, i)                                                                         \
		memSingleListClear(&manager);                                                                              \
	}
#endif

// Double list allocators.
#ifndef MEMORYREGION_EXTEND_ALLOCATORS
#define moduleDefineDoubleList(name, type, manager, freeFunc, size)                           \
	memoryDoubleList manager;                                                                 \
                                                                                              \
	type *module##name##Alloc(){                                                              \
		return(memDoubleListAlloc(&manager));                                                 \
	}                                                                                         \
                                                                                              \
	type *module##name##Prepend(type **const restrict start){                                 \
		return(memDoubleListPrepend(&manager, (void **)start));                               \
	}                                                                                         \
                                                                                              \
	type *module##name##Append(type **const restrict start){                                  \
		return(memDoubleListAppend(&manager, (void **)start));                                \
	}                                                                                         \
                                                                                              \
	type *module##name##InsertBefore(type **const restrict start, type *const restrict next){ \
		return(memDoubleListInsertBefore(&manager, (void **)start, (void *)next));            \
	}                                                                                         \
                                                                                              \
	type *module##name##InsertAfter(type **const restrict start, type *const restrict prev){  \
		return(memDoubleListInsertAfter(&manager, (void **)start, (void *)prev));             \
	}                                                                                         \
                                                                                              \
	type *module##name##Prev(const type *const restrict element){                             \
		return(memDoubleListPrev(element));                                                   \
	}                                                                                         \
                                                                                              \
	type *module##name##Next(const type *const restrict element){                             \
		return(memDoubleListNext(element));                                                   \
	}                                                                                         \
                                                                                              \
	void module##name##Free(type **const restrict start, type *const restrict element){       \
		freeFunc(element);                                                                    \
		memDoubleListFree(&manager, (void **)start, (void *)element);                         \
	}                                                                                         \
                                                                                              \
	void module##name##FreeArray(type **const restrict start){                                \
		type *element = *start;                                                               \
		while(element != NULL){                                                               \
			module##name##Free(start, element);                                               \
			element = *start;                                                                 \
		}                                                                                     \
	}                                                                                         \
                                                                                              \
	void module##name##Clear(){                                                               \
		MEMDOUBLELIST_LOOP_BEGIN(manager, i, type)                                            \
			module##name##Free(NULL, i);                                                      \
		MEMDOUBLELIST_LOOP_END(manager, i)                                                    \
		memDoubleListClear(&manager);                                                         \
	}
#else
#define moduleDefineDoubleList(name, type, manager, freeFunc, size)                           \
	memoryDoubleList manager;                                                                 \
                                                                                              \
	type *module##name##Alloc(){                                                              \
		type *newBlock = memDoubleListAlloc(&manager);                                        \
		if(newBlock == NULL){                                                                 \
			if(memDoubleListExtend(                                                           \
				&manager,                                                                     \
				memoryManagerGlobalAlloc(memoryGetRequiredSize(size)),                        \
				memoryGetRequiredSize(size)                                                   \
			)){                                                                               \
				newBlock = memDoubleListAlloc(&manager);                                      \
			}                                                                                 \
		}                                                                                     \
		return(newBlock);                                                                     \
	}                                                                                         \
                                                                                              \
	type *module##name##Prepend(type **const restrict start){                                 \
		type *newBlock = memDoubleListPrepend(&manager, (void **)start);                      \
		if(newBlock == NULL){                                                                 \
			if(memDoubleListExtend(                                                           \
				&manager,                                                                     \
				memoryManagerGlobalAlloc(memoryGetRequiredSize(size)),                        \
				memoryGetRequiredSize(size)                                                   \
			)){                                                                               \
				newBlock = memDoubleListPrepend(&manager, (void **)start);                    \
			}                                                                                 \
		}                                                                                     \
		return(newBlock);                                                                     \
	}                                                                                         \
                                                                                              \
	type *module##name##Append(type **const restrict start){                                  \
		type *newBlock = memDoubleListAppend(&manager, (void **)start);                       \
		if(newBlock == NULL){                                                                 \
			if(memDoubleListExtend(                                                           \
				&manager,                                                                     \
				memoryManagerGlobalAlloc(memoryGetRequiredSize(size)),                        \
				memoryGetRequiredSize(size)                                                   \
			)){                                                                               \
				newBlock = memDoubleListAppend(&manager, (void **)start);                     \
			}                                                                                 \
		}                                                                                     \
		return(newBlock);                                                                     \
	}                                                                                         \
                                                                                              \
	type *module##name##InsertBefore(type **const restrict start, type *const restrict next){ \
		type *newBlock = memDoubleListInsertBefore(&manager, (void **)start, (void *)next);   \
		if(newBlock == NULL){                                                                 \
			if(memDoubleListExtend(                                                           \
				&manager,                                                                     \
				memoryManagerGlobalAlloc(memoryGetRequiredSize(size)),                        \
				memoryGetRequiredSize(size)                                                   \
			)){                                                                               \
				newBlock = memDoubleListInsertBefore(&manager, (void **)start, (void *)next); \
			}                                                                                 \
		}                                                                                     \
		return(newBlock);                                                                     \
	}                                                                                         \
                                                                                              \
	type *module##name##InsertAfter(type **const restrict start, type *const restrict prev){  \
		type *newBlock = memDoubleListInsertAfter(&manager, (void **)start, (void *)prev);    \
		if(newBlock == NULL){                                                                 \
			if(memDoubleListExtend(                                                           \
				&manager,                                                                     \
				memoryManagerGlobalAlloc(memoryGetRequiredSize(size)),                        \
				memoryGetRequiredSize(size)                                                   \
			)){                                                                               \
				newBlock = memDoubleListInsertAfter(&manager, (void **)start, (void *)prev);  \
			}                                                                                 \
		}                                                                                     \
		return(newBlock);                                                                     \
	}                                                                                         \
                                                                                              \
	type *module##name##Prev(const type *const restrict element){                             \
		return(memDoubleListPrev(element));                                                   \
	}                                                                                         \
                                                                                              \
	type *module##name##Next(const type *const restrict element){                             \
		return(memDoubleListNext(element));                                                   \
	}                                                                                         \
                                                                                              \
	void module##name##Free(type **const restrict start, type *const restrict element){       \
		freeFunc(element);                                                                    \
		memDoubleListFree(&manager, (void **)start, (void *)element);                         \
	}                                                                                         \
                                                                                              \
	void module##name##FreeArray(type **const restrict start){                                \
		type *element = *start;                                                               \
		while(element != NULL){                                                               \
			module##name##Free(start, element);                                               \
			element = *start;                                                                 \
		}                                                                                     \
	}                                                                                         \
                                                                                              \
	void module##name##Clear(){                                                               \
		MEMDOUBLELIST_LOOP_BEGIN(manager, i, type)                                            \
			module##name##Free(NULL, i);                                                      \
		MEMDOUBLELIST_LOOP_END(manager, i)                                                    \
		memDoubleListClear(&manager);                                                         \
	}
#endif


#endif