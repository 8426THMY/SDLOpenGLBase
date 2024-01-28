#include "moduleObject.h"


#include "memoryManager.h"


// objectDef
moduleDefineSingleList(ObjectDef, objectDef, g_objectDefManager, MODULE_OBJECTDEF_MANAGER_SIZE)
moduleDefineSingleListFreeFlexible(ObjectDef, objectDef, g_objectDefManager, objectDefDelete)
// object
moduleDefineSingleList(Object, object, g_objectManager, MODULE_OBJECT_MANAGER_SIZE)
moduleDefineSingleListFreeFlexible(Object, object, g_objectManager, objectDelete)


return_t moduleObjectSetup(){
	return(moduleObjectDefInit() && moduleObjectInit());
}

void moduleObjectCleanup(){
	moduleObjectDelete();
	moduleObjectDefDelete();
}