#include "moduleObject.h"


memorySingleList objectDefManager;
memorySingleList objectManager;


return_t moduleObjectSetup(){
	return(
		// objectDef
		memSingleListInit(
			&objectDefManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_OBJECTDEF_MANAGER_SIZE)),
			MODULE_OBJECTDEF_MANAGER_SIZE, MODULE_OBJECTDEF_ELEMENT_SIZE
		) != NULL &&
		// object
		memSingleListInit(
			&objectManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_OBJECT_MANAGER_SIZE)),
			MODULE_OBJECT_MANAGER_SIZE, MODULE_OBJECT_ELEMENT_SIZE
		) != NULL
	);
}

void moduleObjectCleanup(){
	// object
	moduleObjectClear();
	memoryManagerGlobalFree(memSingleListRegionStart(objectManager.region));
	// objectDef
	moduleObjectDefClear();
	memoryManagerGlobalFree(memSingleListRegionStart(objectDefManager.region));
}


// Allocate a new object base array.
objectDef *moduleObjectDefAlloc(){
	return(memSingleListAlloc(&objectDefManager));
}

// Insert an object base at the beginning of an array.
objectDef *moduleObjectDefPrepend(objectDef **start){
	return(memSingleListPrepend(&objectDefManager, (void **)start));
}

// Insert an object base at the end of an array.
objectDef *moduleObjectDefAppend(objectDef **start){
	return(memSingleListAppend(&objectDefManager, (void **)start));
}

// Insert an object base after the element "prevData".
objectDef *moduleObjectDefInsertBefore(objectDef **start, objectDef *prevData){
	return(memSingleListInsertBefore(&objectDefManager, (void **)start, (void *)prevData));
}

// Insert an object base after the element "data".
objectDef *moduleObjectDefInsertAfter(objectDef **start, objectDef *data){
	return(memSingleListInsertAfter(&objectDefManager, (void **)start, (void *)data));
}

// Free an object base that has been allocated.
void moduleObjectDefFree(objectDef **start, objectDef *objDef, objectDef *prevData){
	objectDefDelete(objDef);
	memSingleListFree(&objectDefManager, (void **)start, (void *)objDef, (void *)prevData);
}

// Free an entire object base array.
void moduleObjectDefFreeArray(objectDef **start){
	objectDef *objDef = *start;
	while(objDef != NULL){
		moduleObjectDefFree(start, objDef, NULL);
		objDef = *start;
	}
}

// Delete every object base in the manager.
void moduleObjectDefClear(){
	MEMSINGLELIST_LOOP_BEGIN(objectDefManager, i, objectDef *)
		moduleObjectDefFree(NULL, i, NULL);
	MEMSINGLELIST_LOOP_END(objectDefManager, i, objectDef *, return)
}


// Allocate a new object array.
object *moduleObjectAlloc(){
	return(memSingleListAlloc(&objectManager));
}

// Insert an object at the beginning of an array.
object *moduleObjectPrepend(object **start){
	return(memSingleListPrepend(&objectManager, (void **)start));
}

// Insert an object at the end of an array.
object *moduleObjectAppend(object **start){
	return(memSingleListAppend(&objectManager, (void **)start));
}

// Insert an object after the element "prevData".
object *moduleObjectInsertBefore(object **start, object *prevData){
	return(memSingleListInsertBefore(&objectManager, (void **)start, (void *)prevData));
}

// Insert an object after the element "data".
object *moduleObjectInsertAfter(object **start, object *data){
	return(memSingleListInsertAfter(&objectManager, (void **)start, (void *)data));
}

// Free an object that has been allocated.
void moduleObjectFree(object **start, object *obj, object *prevData){
	objectDelete(obj);
	memSingleListFree(&objectManager, (void **)start, (void *)obj, (void *)prevData);
}

// Free an entire object array.
void moduleObjectFreeArray(object **start){
	object *obj = *start;
	while(obj != NULL){
		moduleObjectFree(start, obj, NULL);
		obj = *start;
	}
}

// Delete every object in the manager.
void moduleObjectClear(){
	MEMSINGLELIST_LOOP_BEGIN(objectManager, i, object *)
		moduleObjectFree(NULL, i, NULL);
	MEMSINGLELIST_LOOP_END(objectManager, i, object *, return)
}