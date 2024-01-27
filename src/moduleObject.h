#ifndef moduleObject_h
#define moduleObject_h


#include "object.h"

#include "utilTypes.h"
#include "moduleShared.h"


#define MODULE_OBJECT
#define MODULE_OBJECT_SETUP_FAIL 7

#define MODULE_OBJECTDEF_ELEMENT_SIZE sizeof(objectDef)
#define MODULE_OBJECT_ELEMENT_SIZE    sizeof(object)

#ifndef MEMORY_MODULE_NUM_OBJECTDEFS
	#define MEMORY_MODULE_NUM_OBJECTDEFS 1
#endif
#ifndef MEMORY_MODULE_NUM_OBJECTS
	#define MEMORY_MODULE_NUM_OBJECTS 1
#endif

#define MODULE_OBJECTDEF_MANAGER_SIZE \
	memSingleListMemoryForBlocks(MEMORY_MODULE_NUM_OBJECTDEFS, MODULE_OBJECTDEF_ELEMENT_SIZE)
#define MODULE_OBJECT_MANAGER_SIZE \
	memSingleListMemoryForBlocks(MEMORY_MODULE_NUM_OBJECTS, MODULE_OBJECT_ELEMENT_SIZE)


moduleDeclareSingleList(ObjectDef, objectDef, g_objectDefManager)
moduleDeclareSingleList(Object, object, g_objectManager)

return_t moduleObjectSetup();
void moduleObjectCleanup();


#endif