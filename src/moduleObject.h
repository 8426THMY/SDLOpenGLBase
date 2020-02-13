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
objectDef *moduleObjectDefPrepend(objectDef **const restrict start);
objectDef *moduleObjectDefAppend(objectDef **const restrict start);
objectDef *moduleObjectDefInsertBefore(objectDef **const restrict start, objectDef *const restrict prevData);
objectDef *moduleObjectDefInsertAfter(objectDef **const restrict start, objectDef *const restrict data);
void moduleObjectDefFree(objectDef **const restrict start, objectDef *const restrict objDef, objectDef *const restrict prevData);
void moduleObjectDefFreeArray(objectDef **const restrict start);
void moduleObjectDefClear();

object *moduleObjectAlloc();
object *moduleObjectPrepend(object **const restrict start);
object *moduleObjectAppend(object **const restrict start);
object *moduleObjectInsertBefore(object **const restrict start, object *const restrict prevData);
object *moduleObjectInsertAfter(object **const restrict start, object *const restrict data);
void moduleObjectFree(object **const restrict start, object *const restrict obj, object *const restrict prevData);
void moduleObjectFreeArray(object **const restrict start);
void moduleObjectClear();


extern memorySingleList g_objectDefManager;
extern memorySingleList g_objectManager;


#endif