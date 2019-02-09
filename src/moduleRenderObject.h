#ifndef renderObject_h
#define renderObject_h


#include "utilTypes.h"
#include "vector.h"

#include "renderObject.h"


#define MODULERENDEROBJ
#define MODULERENDEROBJ_SETUP_FAIL 5

#ifndef MEMORY_MAX_RENDEROBJS
	#define MEMORY_MAX_RENDEROBJS 1
#endif


return_t moduleRenderObjSetup();
void moduleRenderObjCleanup();


extern vector allRenderObjects;


#endif