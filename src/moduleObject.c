#include "moduleObject.h"


memorySingleList g_objectDefManager;
memorySingleList g_objectManager;


return_t moduleObjectSetup(){
	return(
		// objectDef
		memSingleListInit(
			&g_objectDefManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_OBJECTDEF_MANAGER_SIZE)),
			MODULE_OBJECTDEF_MANAGER_SIZE, MODULE_OBJECTDEF_ELEMENT_SIZE
		) != NULL &&
		// object
		memSingleListInit(
			&g_objectManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_OBJECT_MANAGER_SIZE)),
			MODULE_OBJECT_MANAGER_SIZE, MODULE_OBJECT_ELEMENT_SIZE
		) != NULL
	);
}

void moduleObjectCleanup(){
	// object
	MEMSINGLELIST_LOOP_BEGIN(g_objectManager, i, object)
		moduleObjectFree(NULL, i, NULL);
	MEMSINGLELIST_LOOP_END(g_objectManager, i)
	memSingleListDelete(&g_objectManager, memoryManagerGlobalFree);
	// objectDef
	MEMSINGLELIST_LOOP_BEGIN(g_objectDefManager, i, objectDef)
		moduleObjectDefFree(NULL, i, NULL);
	MEMSINGLELIST_LOOP_END(g_objectDefManager, i)
	memSingleListDelete(&g_objectDefManager, memoryManagerGlobalFree);
}


// Allocate a new object base array.
objectDef *moduleObjectDefAlloc(){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memSingleListAlloc(&g_objectDefManager));
	#else
	objectDef *newBlock = memSingleListAlloc(&g_objectDefManager);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memSingleListExtend(
			&g_objectDefManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_OBJECTDEF_MANAGER_SIZE)),
			MODULE_OBJECTDEF_MANAGER_SIZE
		)){
			newBlock = memSingleListAlloc(&g_objectDefManager);
		}
	}
	return(newBlock);
	#endif
}

// Insert an object base at the beginning of an array.
objectDef *moduleObjectDefPrepend(objectDef **const restrict start){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memSingleListPrepend(&g_objectDefManager, (void **)start));
	#else
	objectDef *newBlock = memSingleListPrepend(&g_objectDefManager, (void **)start);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memSingleListExtend(
			&g_objectDefManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_OBJECTDEF_MANAGER_SIZE)),
			MODULE_OBJECTDEF_MANAGER_SIZE
		)){
			newBlock = memSingleListPrepend(&g_objectDefManager, (void **)start);
		}
	}
	return(newBlock);
	#endif
}

// Insert an object base at the end of an array.
objectDef *moduleObjectDefAppend(objectDef **const restrict start){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memSingleListAppend(&g_objectDefManager, (void **)start));
	#else
	objectDef *newBlock = memSingleListAppend(&g_objectDefManager, (void **)start);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memSingleListExtend(
			&g_objectDefManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_OBJECTDEF_MANAGER_SIZE)),
			MODULE_OBJECTDEF_MANAGER_SIZE
		)){
			newBlock = memSingleListAppend(&g_objectDefManager, (void **)start);
		}
	}
	return(newBlock);
	#endif
}

// Insert an object base after the element "prevData".
objectDef *moduleObjectDefInsertBefore(objectDef **const restrict start, objectDef *const restrict prevData){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memSingleListInsertBefore(&g_objectDefManager, (void **)start, (void *)prevData));
	#else
	objectDef *newBlock = memSingleListInsertBefore(&g_objectDefManager, (void **)start, (void *)prevData);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memSingleListExtend(
			&g_objectDefManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_OBJECTDEF_MANAGER_SIZE)),
			MODULE_OBJECTDEF_MANAGER_SIZE
		)){
			newBlock = memSingleListInsertBefore(&g_objectDefManager, (void **)start, (void *)prevData);
		}
	}
	return(newBlock);
	#endif
}

// Insert an object base after the element "data".
objectDef *moduleObjectDefInsertAfter(objectDef **const restrict start, objectDef *const restrict data){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memSingleListInsertAfter(&g_objectDefManager, (void **)start, (void *)data));
	#else
	objectDef *newBlock = memSingleListInsertAfter(&g_objectDefManager, (void **)start, (void *)data);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memSingleListExtend(
			&g_objectDefManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_OBJECTDEF_MANAGER_SIZE)),
			MODULE_OBJECTDEF_MANAGER_SIZE
		)){
			newBlock = memSingleListInsertAfter(&g_objectDefManager, (void **)start, (void *)data);
		}
	}
	return(newBlock);
	#endif
}

objectDef *moduleObjectDefNext(const objectDef *const restrict objDef){
	return(memSingleListNext(objDef));
}

// Free an object base that has been allocated.
void moduleObjectDefFree(objectDef **const restrict start, objectDef *const restrict objDef, objectDef *const restrict prevData){
	objectDefDelete(objDef);
	memSingleListFree(&g_objectDefManager, (void **)start, (void *)objDef, (void *)prevData);
}

// Free an entire object base array.
void moduleObjectDefFreeArray(objectDef **const restrict start){
	objectDef *objDef = *start;
	while(objDef != NULL){
		moduleObjectDefFree(start, objDef, NULL);
		objDef = *start;
	}
}

// Delete every object base in the manager.
void moduleObjectDefClear(){
	MEMSINGLELIST_LOOP_BEGIN(g_objectDefManager, i, objectDef)
		moduleObjectDefFree(NULL, i, NULL);
	MEMSINGLELIST_LOOP_END(g_objectDefManager, i)
	memSingleListClear(&g_objectDefManager);
}


// Allocate a new object array.
object *moduleObjectAlloc(){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memSingleListAlloc(&g_objectManager));
	#else
	object *newBlock = memSingleListAlloc(&g_objectManager);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memSingleListExtend(
			&g_objectManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_OBJECT_MANAGER_SIZE)),
			MODULE_OBJECT_MANAGER_SIZE
		)){
			newBlock = memSingleListAlloc(&g_objectManager);
		}
	}
	return(newBlock);
	#endif
}

// Insert an object at the beginning of an array.
object *moduleObjectPrepend(object **const restrict start){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memSingleListPrepend(&g_objectManager, (void **)start));
	#else
	object *newBlock = memSingleListPrepend(&g_objectManager, (void **)start);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memSingleListExtend(
			&g_objectManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_OBJECT_MANAGER_SIZE)),
			MODULE_OBJECT_MANAGER_SIZE
		)){
			newBlock = memSingleListPrepend(&g_objectManager, (void **)start);
		}
	}
	return(newBlock);
	#endif
}

// Insert an object at the end of an array.
object *moduleObjectAppend(object **const restrict start){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memSingleListAppend(&g_objectManager, (void **)start));
	#else
	object *newBlock = memSingleListAppend(&g_objectManager, (void **)start);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memSingleListExtend(
			&g_objectManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_OBJECT_MANAGER_SIZE)),
			MODULE_OBJECT_MANAGER_SIZE
		)){
			newBlock = memSingleListAppend(&g_objectManager, (void **)start);
		}
	}
	return(newBlock);
	#endif
}

// Insert an object after the element "prevData".
object *moduleObjectInsertBefore(object **const restrict start, object *const restrict prevData){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memSingleListInsertBefore(&g_objectManager, (void **)start, (void *)prevData));
	#else
	object *newBlock = memSingleListInsertBefore(&g_objectManager, (void **)start, (void *)prevData);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memSingleListExtend(
			&g_objectManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_OBJECT_MANAGER_SIZE)),
			MODULE_OBJECT_MANAGER_SIZE
		)){
			newBlock = memSingleListInsertBefore(&g_objectManager, (void **)start, (void *)prevData);
		}
	}
	return(newBlock);
	#endif
}

// Insert an object after the element "data".
object *moduleObjectInsertAfter(object **const restrict start, object *const restrict data){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memSingleListInsertAfter(&g_objectManager, (void **)start, (void *)data));
	#else
	object *newBlock = memSingleListInsertAfter(&g_objectManager, (void **)start, (void *)data);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memSingleListExtend(
			&g_objectManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_OBJECT_MANAGER_SIZE)),
			MODULE_OBJECT_MANAGER_SIZE
		)){
			newBlock = memSingleListInsertAfter(&g_objectManager, (void **)start, (void *)data);
		}
	}
	return(newBlock);
	#endif
}

object *moduleObjectNext(const object *const restrict obj){
	return(memSingleListNext(obj));
}

// Free an object that has been allocated.
void moduleObjectFree(object **const restrict start, object *const restrict obj, object *const restrict prevData){
	objectDelete(obj);
	memSingleListFree(&g_objectManager, (void **)start, (void *)obj, (void *)prevData);
}

// Free an entire object array.
void moduleObjectFreeArray(object **const restrict start){
	object *obj = *start;
	while(obj != NULL){
		moduleObjectFree(start, obj, NULL);
		obj = *start;
	}
}

// Delete every object in the manager.
void moduleObjectClear(){
	MEMSINGLELIST_LOOP_BEGIN(g_objectManager, i, object)
		moduleObjectFree(NULL, i, NULL);
	MEMSINGLELIST_LOOP_END(g_objectManager, i)
	memSingleListClear(&g_objectManager);
}