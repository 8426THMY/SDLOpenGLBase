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
	moduleObjectClear();
	memoryManagerGlobalFree(memSingleListRegionStart(g_objectManager.region));
	// objectDef
	moduleObjectDefClear();
	memoryManagerGlobalFree(memSingleListRegionStart(g_objectDefManager.region));
}


// Allocate a new object base array.
objectDef *moduleObjectDefAlloc(){
	return(memSingleListAlloc(&g_objectDefManager));
}

// Insert an object base at the beginning of an array.
objectDef *moduleObjectDefPrepend(objectDef **const restrict start){
	return(memSingleListPrepend(&g_objectDefManager, (void **)start));
}

// Insert an object base at the end of an array.
objectDef *moduleObjectDefAppend(objectDef **const restrict start){
	return(memSingleListAppend(&g_objectDefManager, (void **)start));
}

// Insert an object base after the element "prevData".
objectDef *moduleObjectDefInsertBefore(objectDef **const restrict start, objectDef *const restrict prevData){
	return(memSingleListInsertBefore(&g_objectDefManager, (void **)start, (void *)prevData));
}

// Insert an object base after the element "data".
objectDef *moduleObjectDefInsertAfter(objectDef **const restrict start, objectDef *const restrict data){
	return(memSingleListInsertAfter(&g_objectDefManager, (void **)start, (void *)data));
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
	MEMSINGLELIST_LOOP_BEGIN(g_objectDefManager, i, objectDef *)
		moduleObjectDefFree(NULL, i, NULL);
	MEMSINGLELIST_LOOP_END(g_objectDefManager, i, objectDef *, return)
}


// Allocate a new object array.
object *moduleObjectAlloc(){
	return(memSingleListAlloc(&g_objectManager));
}

// Insert an object at the beginning of an array.
object *moduleObjectPrepend(object **const restrict start){
	return(memSingleListPrepend(&g_objectManager, (void **)start));
}

// Insert an object at the end of an array.
object *moduleObjectAppend(object **const restrict start){
	return(memSingleListAppend(&g_objectManager, (void **)start));
}

// Insert an object after the element "prevData".
object *moduleObjectInsertBefore(object **const restrict start, object *const restrict prevData){
	return(memSingleListInsertBefore(&g_objectManager, (void **)start, (void *)prevData));
}

// Insert an object after the element "data".
object *moduleObjectInsertAfter(object **const restrict start, object *const restrict data){
	return(memSingleListInsertAfter(&g_objectManager, (void **)start, (void *)data));
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
	MEMSINGLELIST_LOOP_BEGIN(g_objectManager, i, object *)
		moduleObjectFree(NULL, i, NULL);
	MEMSINGLELIST_LOOP_END(g_objectManager, i, object *, return)
}