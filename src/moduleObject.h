#ifndef moduleObject_h
#define moduleObject_h


#include "utilTypes.h"

#include "memoryManager.h"
#include "memorySingleList.h"

#include "object.h"


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

#define MODULE_OBJECTDEF_MANAGER_SIZE memSingleListMemoryForBlocks(MEMORY_MODULE_NUM_OBJECTDEFS, MODULE_OBJECTDEF_ELEMENT_SIZE)
#define MODULE_OBJECT_MANAGER_SIZE    memSingleListMemoryForBlocks(MEMORY_MODULE_NUM_OBJECTS, MODULE_OBJECT_ELEMENT_SIZE)


return_t moduleObjectSetup();
void moduleObjectCleanup();

objectDef *moduleObjectDefAlloc();
objectDef *moduleObjectDefPrepend(objectDef **start);
objectDef *moduleObjectDefAppend(objectDef **start);
objectDef *moduleObjectDefInsertBefore(objectDef **start, objectDef *prevData);
objectDef *moduleObjectDefInsertAfter(objectDef **start, objectDef *data);
void moduleObjectDefFree(objectDef **start, objectDef *objDef, objectDef *prevData);
void moduleObjectDefFreeArray(objectDef **start);
void moduleObjectDefClear();

object *moduleObjectAlloc();
object *moduleObjectPrepend(object **start);
object *moduleObjectAppend(object **start);
object *moduleObjectInsertBefore(object **start, object *prevData);
object *moduleObjectInsertAfter(object **start, object *data);
void moduleObjectFree(object **start, object *obj, object *prevData);
void moduleObjectFreeArray(object **start);
void moduleObjectClear();


extern memorySingleList objectDefManager;
extern memorySingleList objectManager;


#endif