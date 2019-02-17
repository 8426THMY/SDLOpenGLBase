#ifndef moduleRenderObject_h
#define moduleRenderObject_h


#include "utilTypes.h"
#include "vector.h"

#include "renderObject.h"


#define MODULE_RENDEROBJ
#define MODULE_RENDEROBJ_SETUP_FAIL 6

#define MODULE_RENDEROBJ_ELEMENT_SIZE sizeof(renderObjState)

#ifndef MEMORY_MODULE_NUM_RENDEROBJS
	#define MEMORY_MODULE_NUM_RENDEROBJS 1
#endif

#define MODULE_RENDEROBJ_MANAGER_SIZE memPoolMemoryForBlocks(MEMORY_MODULE_NUM_RENDEROBJS, MODULE_RENDEROBJ_ELEMENT_SIZE)


return_t moduleRenderObjSetup();
void moduleRenderObjCleanup();


extern vector allRenderObjects;


#endif