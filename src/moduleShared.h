#ifndef moduleShared_h
#define moduleShared_h


/*
** These function macros allow us to declare
** prototypes for custom module functions.
*/

// Pool allocators.
#define moduleDeclarePool(name, type, manager) \
	return_t module##name##Init();             \
	type *module##name##Alloc();               \
	extern memoryPool manager;
#define moduleDeclarePoolFree(name, type)                  \
	void module##name##Free(type *const restrict element); \
	void module##name##Clear();                            \
	void module##name##Delete();

// Single list allocators.
#define moduleDeclareSingleList(name, type, manager)                                         \
	return_t module##name##Init();                                                           \
	type *module##name##Alloc();                                                             \
	type *module##name##Prepend(type **const restrict start);                                \
	type *module##name##Append(type **const restrict start);                                 \
	type *module##name##InsertAfter(type **const restrict start, type *const restrict prev); \
	type *module##name##Next(const type *const restrict element);                            \
	extern memorySingleList manager;
#define moduleDeclareSingleListFree(name, type)                                                                    \
	void module##name##Free(type **const restrict start, type *const restrict element, type *const restrict prev); \
	void module##name##FreeArray(type **const restrict start);                                                     \
	void module##name##Clear();                                                                                    \
	void module##name##Delete();

// Double list allocators.
#define moduleDeclareDoubleList(name, type, manager)                                          \
	return_t module##name##Init();                                                            \
	type *module##name##Alloc();                                                              \
	type *module##name##Prepend(type **const restrict start);                                 \
	type *module##name##Append(type **const restrict start);                                  \
	type *module##name##InsertBefore(type **const restrict start, type *const restrict next); \
	type *module##name##InsertAfter(type **const restrict start, type *const restrict prev);  \
	type *module##name##Prev(const type *const restrict element);                             \
	type *module##name##Next(const type *const restrict element);                             \
	extern memoryDoubleList manager;
#define moduleDeclareDoubleListFree(name, type)                                         \
	void module##name##Free(type **const restrict start, type *const restrict element); \
	void module##name##Array(type **const restrict start);                              \
	void module##name##Clear();                                                         \
	void module##name##Delete();

/*
** Assuming function prototypes have been created using
** the macros above, these macros provide the definitions.
*/

// Pool allocators.
#ifndef MEMORYREGION_EXTEND_ALLOCATORS
#define moduleDefinePool(name, type, manager, size)                    \
	memoryPool manager;                                                \
                                                                       \
	return_t module##name##Init(){                                     \
		return(                                                        \
			memPoolInit(                                               \
				&manager,                                              \
				memoryManagerGlobalAlloc(memoryGetRequiredSize(size)), \
				memoryGetRequiredSize(size), sizeof(type)              \
			) != NULL                                                  \
		);                                                             \
	}                                                                  \
                                                                       \
	type *module##name##Alloc(){                                       \
		return(memPoolAlloc(&manager));                                \
	}
#else
#define moduleDefinePool(name, type, manager, size)                    \
	memoryPool manager;                                                \
                                                                       \
	return_t module##name##Init(){                                     \
		return(                                                        \
			memPoolInit(                                               \
				&manager,                                              \
				memoryManagerGlobalAlloc(memoryGetRequiredSize(size)), \
				memoryGetRequiredSize(size), sizeof(type)              \
			) != NULL                                                  \
		);                                                             \
	}                                                                  \
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
	}
#endif
#define moduleDefinePoolFree(name, type, manager)          \
	void module##name##Free(type *const restrict element){ \
		memPoolFree(&manager, element);                    \
	}                                                      \
                                                           \
	void module##name##Clear(){                            \
		MEMPOOL_LOOP_BEGIN(manager, i, type)               \
			module##name##Free(i);                         \
		MEMPOOL_LOOP_END(manager, i)                       \
		memPoolClear(&manager);                            \
	}                                                      \
                                                           \
	void module##name##Delete(){                           \
		MEMPOOL_LOOP_BEGIN(manager, i, type)               \
			module##name##Free(i);                         \
		MEMPOOL_LOOP_END(manager, i)                       \
		memoryManagerGlobalDeleteRegions(manager.region);  \
	}
#define moduleDefinePoolFreeFlexible(name, type, manager, func) \
	void module##name##Free(type *const restrict element){      \
		func(element);                                          \
		memPoolFree(&manager, element);                         \
	}                                                           \
                                                                \
	void module##name##Clear(){                                 \
		MEMPOOL_LOOP_BEGIN(manager, i, type)                    \
			module##name##Free(i);                              \
		MEMPOOL_LOOP_END(manager, i)                            \
		memPoolClear(&manager);                                 \
	}                                                           \
                                                                \
	void module##name##Delete(){                                \
		MEMPOOL_LOOP_BEGIN(manager, i, type)                    \
			module##name##Free(i);                              \
		MEMPOOL_LOOP_END(manager, i)                            \
		memoryManagerGlobalDeleteRegions(manager.region);       \
	}

// Single list allocators.
#ifndef MEMORYREGION_EXTEND_ALLOCATORS
#define moduleDefineSingleList(name, type, manager, size)                                    \
	memorySingleList manager;                                                                \
                                                                                             \
	return_t module##name##Init(){                                                           \
		return(                                                                              \
			memSingleListInit(                                                               \
				&manager,                                                                    \
				memoryManagerGlobalAlloc(memoryGetRequiredSize(size)),                       \
				memoryGetRequiredSize(size), sizeof(type)                                    \
			) != NULL                                                                        \
		);                                                                                   \
	}                                                                                        \
                                                                                             \
	type *module##name##Alloc(){                                                             \
		return(memSingleListAlloc(&manager));                                                \
	}                                                                                        \
                                                                                             \
	type *module##name##Prepend(type **const restrict start){                                \
		return(memSingleListPrepend(&manager, (void **)start));                              \
	}                                                                                        \
                                                                                             \
	type *module##name##Append(type **const restrict start){                                 \
		return(memSingleListAppend(&manager, (void **)start));                               \
	}                                                                                        \
                                                                                             \
	type *module##name##InsertAfter(type **const restrict start, type *const restrict prev){ \
		return(memSingleListInsertAfter(&manager, (void **)start, (void *)prev));            \
	}                                                                                        \
                                                                                             \
	type *module##name##Next(const type *const restrict element){                            \
		return(memSingleListNext(element));                                                  \
	}
#else
#define moduleDefineSingleList(name, type, manager, size)                                    \
	memorySingleList manager;                                                                \
                                                                                             \
	return_t module##name##Init(){                                                           \
		return(                                                                              \
			memSingleListInit(                                                               \
				&manager,                                                                    \
				memoryManagerGlobalAlloc(memoryGetRequiredSize(size)),                       \
				memoryGetRequiredSize(size), sizeof(type)                                    \
			) != NULL                                                                        \
		);                                                                                   \
	}                                                                                        \
                                                                                             \
	type *module##name##Alloc(){                                                             \
		type *newBlock = memSingleListAlloc(&manager);                                       \
		if(newBlock == NULL){                                                                \
			if(memSingleListExtend(                                                          \
				&manager,                                                                    \
				memoryManagerGlobalAlloc(memoryGetRequiredSize(size)),                       \
				memoryGetRequiredSize(size)                                                  \
			)){                                                                              \
				newBlock = memSingleListAlloc(&manager);                                     \
			}                                                                                \
		}                                                                                    \
		return(newBlock);                                                                    \
	}                                                                                        \
                                                                                             \
	type *module##name##Prepend(type **const restrict start){                                \
		type *newBlock = memSingleListPrepend(&manager, (void **)start);                     \
		if(newBlock == NULL){                                                                \
			if(memSingleListExtend(                                                          \
				&manager,                                                                    \
				memoryManagerGlobalAlloc(memoryGetRequiredSize(size)),                       \
				memoryGetRequiredSize(size)                                                  \
			)){                                                                              \
				newBlock = memSingleListPrepend(&manager, (void **)start);                   \
			}                                                                                \
		}                                                                                    \
		return(newBlock);                                                                    \
	}                                                                                        \
                                                                                             \
	type *module##name##Append(type **const restrict start){                                 \
		type *newBlock = memSingleListAppend(&manager, (void **)start);                      \
		if(newBlock == NULL){                                                                \
			if(memSingleListExtend(                                                          \
				&manager,                                                                    \
				memoryManagerGlobalAlloc(memoryGetRequiredSize(size)),                       \
				memoryGetRequiredSize(size)                                                  \
			)){                                                                              \
				newBlock = memSingleListAppend(&manager, (void **)start);                    \
			}                                                                                \
		}                                                                                    \
		return(newBlock);                                                                    \
	}                                                                                        \
                                                                                             \
	type *module##name##InsertAfter(type **const restrict start, type *const restrict prev){ \
		type *newBlock = memSingleListInsertAfter(&manager, (void **)start, (void *)prev);   \
		if(newBlock == NULL){                                                                \
			if(memSingleListExtend(                                                          \
				&manager,                                                                    \
				memoryManagerGlobalAlloc(memoryGetRequiredSize(size)),                       \
				memoryGetRequiredSize(size)                                                  \
			)){                                                                              \
				newBlock = memSingleListInsertAfter(&manager, (void **)start, (void *)prev); \
			}                                                                                \
		}                                                                                    \
		return(newBlock);                                                                    \
	}                                                                                        \
                                                                                             \
	type *module##name##Next(const type *const restrict element){                            \
		return(memSingleListNext(element));                                                  \
	}
#endif
#define moduleDefineSingleListFree(name, type, manager)                                                            \
	void module##name##Free(type **const restrict start, type *const restrict element, type *const restrict prev){ \
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
	}                                                                                                              \
                                                                                                                   \
	void module##name##Delete(){                                                                                   \
		MEMSINGLELIST_LOOP_BEGIN(manager, i, type)                                                                 \
			module##name##Free(NULL, i, NULL);                                                                     \
		MEMSINGLELIST_LOOP_END(manager, i)                                                                         \
		memoryManagerGlobalDeleteRegions(manager.region);                                                          \
	}
#define moduleDefineSingleListFreeFlexible(name, type, manager, func)                                              \
	void module##name##Free(type **const restrict start, type *const restrict element, type *const restrict prev){ \
		func(element);                                                                                             \
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
	}                                                                                                              \
                                                                                                                   \
	void module##name##Delete(){                                                                                   \
		MEMSINGLELIST_LOOP_BEGIN(manager, i, type)                                                                 \
			module##name##Free(NULL, i, NULL);                                                                     \
		MEMSINGLELIST_LOOP_END(manager, i)                                                                         \
		memoryManagerGlobalDeleteRegions(manager.region);                                                          \
	}

// Double list allocators.
#ifndef MEMORYREGION_EXTEND_ALLOCATORS
#define moduleDefineDoubleList(name, type, manager, size)                                     \
	memoryDoubleList manager;                                                                 \
                                                                                              \
	return_t module##name##Init(){                                                            \
		return(                                                                               \
			memDoubleListInit(                                                                \
				&manager,                                                                     \
				memoryManagerGlobalAlloc(memoryGetRequiredSize(size)),                        \
				memoryGetRequiredSize(size), sizeof(type)                                     \
			) != NULL                                                                         \
		);                                                                                    \
	}                                                                                         \
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
	}
#else
#define moduleDefineDoubleList(name, type, manager, size)                                     \
	memoryDoubleList manager;                                                                 \
                                                                                              \
	return_t module##name##Init(){                                                            \
		return(                                                                               \
			memDoubleListInit(                                                                \
				&manager,                                                                     \
				memoryManagerGlobalAlloc(memoryGetRequiredSize(size)),                        \
				memoryGetRequiredSize(size), sizeof(type)                                     \
			) != NULL                                                                         \
		);                                                                                    \
	}                                                                                         \
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
	}
#endif
#define moduleDefineDoubleListFree(name, type, manager)                                 \
	void module##name##Free(type **const restrict start, type *const restrict element){ \
		memDoubleListFree(&manager, (void **)start, (void *)element);                   \
	}                                                                                   \
                                                                                        \
	void module##name##FreeArray(type **const restrict start){                          \
		type *element = *start;                                                         \
		while(element != NULL){                                                         \
			module##name##Free(start, element);                                         \
			element = *start;                                                           \
		}                                                                               \
	}                                                                                   \
                                                                                        \
	void module##name##Clear(){                                                         \
		MEMDOUBLELIST_LOOP_BEGIN(manager, i, type)                                      \
			module##name##Free(NULL, i);                                                \
		MEMDOUBLELIST_LOOP_END(manager, i)                                              \
		memDoubleListClear(&manager);                                                   \
	}                                                                                   \
                                                                                        \
	void module##name##Delete(){                                                        \
		MEMDOUBLELIST_LOOP_BEGIN(manager, i, type)                                      \
			module##name##Free(NULL, i);                                                \
		MEMDOUBLELIST_LOOP_END(manager, i)                                              \
		memoryManagerGlobalDeleteRegions(manager.region);                               \
	}
#define moduleDefineDoubleListFreeFlexible(name, type, manager, func)                   \
	void module##name##Free(type **const restrict start, type *const restrict element){ \
		func(element);                                                                  \
		memDoubleListFree(&manager, (void **)start, (void *)element);                   \
	}                                                                                   \
                                                                                        \
	void module##name##FreeArray(type **const restrict start){                          \
		type *element = *start;                                                         \
		while(element != NULL){                                                         \
			module##name##Free(start, element);                                         \
			element = *start;                                                           \
		}                                                                               \
	}                                                                                   \
                                                                                        \
	void module##name##Clear(){                                                         \
		MEMDOUBLELIST_LOOP_BEGIN(manager, i, type)                                      \
			module##name##Free(NULL, i);                                                \
		MEMDOUBLELIST_LOOP_END(manager, i)                                              \
		memDoubleListClear(&manager);                                                   \
	}                                                                                   \
                                                                                        \
	void module##name##Delete(){                                                        \
		MEMDOUBLELIST_LOOP_BEGIN(manager, i, type)                                      \
			module##name##Free(NULL, i);                                                \
		MEMDOUBLELIST_LOOP_END(manager, i)                                              \
		memoryManagerGlobalDeleteRegions(manager.region);                               \
	}


#endif