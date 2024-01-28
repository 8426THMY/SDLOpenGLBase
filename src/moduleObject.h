#ifndef moduleObject_h
#define moduleObject_h


#include "object.h"

#include "utilTypes.h"
#include "moduleShared.h"


#define MODULE_OBJECT
#define MODULE_OBJECT_SETUP_FAIL 7

#ifndef MEMORY_MODULE_NUM_OBJECTDEFS
	#define MEMORY_MODULE_NUM_OBJECTDEFS 1
#endif
#ifndef MEMORY_MODULE_NUM_OBJECTS
	#define MEMORY_MODULE_NUM_OBJECTS 1
#endif

#define MODULE_OBJECTDEF_MANAGER_SIZE \
	memSingleListMemoryForBlocks(MEMORY_MODULE_NUM_OBJECTDEFS, sizeof(objectDef))
#define MODULE_OBJECT_MANAGER_SIZE \
	memSingleListMemoryForBlocks(MEMORY_MODULE_NUM_OBJECTS, sizeof(object))


// objectDef
moduleDeclareSingleList(ObjectDef, objectDef, g_objectDefManager)
moduleDeclareSingleListFree(ObjectDef, objectDef)
// object
moduleDeclareSingleList(Object, object, g_objectManager)
moduleDeclareSingleListFree(Object, object)

return_t moduleObjectSetup();
void moduleObjectCleanup();


#endif