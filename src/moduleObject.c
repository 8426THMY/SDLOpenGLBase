#include "moduleObject.h"


#include "memoryManager.h"


moduleDefineSingleList(ObjectDef, objectDef, g_objectDefManager, objectDefDelete, MODULE_OBJECTDEF_MANAGER_SIZE)
moduleDefineSingleList(Object, object, g_objectManager, objectDelete, MODULE_OBJECT_MANAGER_SIZE)


return_t moduleObjectSetup(){
	// The module's setup will be successful if we
	// can allocate enough memory for our managers.
	return(
		// objectDef
		memSingleListInit(
			&g_objectDefManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_OBJECTDEF_MANAGER_SIZE)),
			memoryGetRequiredSize(MODULE_OBJECTDEF_MANAGER_SIZE), MODULE_OBJECTDEF_ELEMENT_SIZE
		) != NULL &&
		// object
		memSingleListInit(
			&g_objectManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_OBJECT_MANAGER_SIZE)),
			memoryGetRequiredSize(MODULE_OBJECT_MANAGER_SIZE), MODULE_OBJECT_ELEMENT_SIZE
		) != NULL
	);
}

void moduleObjectCleanup(){
	// object
	MEMSINGLELIST_LOOP_BEGIN(g_objectManager, i, object)
		moduleObjectFree(NULL, i, NULL);
	MEMSINGLELIST_LOOP_END(g_objectManager, i)
	memoryManagerGlobalDeleteRegions(g_objectManager.region);
	// objectDef
	MEMSINGLELIST_LOOP_BEGIN(g_objectDefManager, i, objectDef)
		moduleObjectDefFree(NULL, i, NULL);
	MEMSINGLELIST_LOOP_END(g_objectDefManager, i)
	memoryManagerGlobalDeleteRegions(g_objectDefManager.region);
}