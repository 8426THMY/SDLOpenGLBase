#include "moduleObject.h"


#include "memoryManager.h"


// objectDef
moduleDefineSingleList(ObjectDef, objectDef, g_objectDefManager, MODULE_OBJECTDEF_MANAGER_SIZE)
moduleDefineSingleListFreeFlexible(ObjectDef, objectDef, g_objectDefManager, objectDefDelete)
// object
moduleDefineSingleList(Object, object, g_objectManager, MODULE_OBJECT_MANAGER_SIZE)
moduleDefineSingleListFreeFlexible(Object, object, g_objectManager, objectDelete)


return_t moduleObjectSetup(){
	// The module's setup will be successful if we
	// can allocate enough memory for our managers.
	return(moduleObjectDefInit() && moduleObjectInit());
}

void moduleObjectCleanup(){
	moduleObjectDelete();
	moduleObjectDefDelete();
}