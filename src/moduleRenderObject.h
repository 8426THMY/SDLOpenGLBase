#ifndef renderObject_h
#define renderObject_h


#define MODULERENDEROBJ

#define MODULERENDEROBJ_SETUP_FAIL 5


#include "utilTypes.h"
#include "vector.h"

#include "renderObject.h"


return_t moduleRenderObjSetup();
void moduleRenderObjCleanup();


extern vector allRenderObjects;


#endif