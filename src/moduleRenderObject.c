#include "moduleRenderObject.h"


vector allRenderObjects;


return_t moduleRenderObjSetup(){
	vectorInit(&allRenderObjects, MODULE_RENDEROBJ_ELEMENT_SIZE);

	//We don't need to create an error renderObject since it's
	//basically just a wrapper for models and textureGroups.

	return(1);
}

void moduleRenderObjCleanup(){
	size_t i;
	for(i = 0; i < allRenderObjects.size; ++i){
		renderObjDelete((renderObject *)vectorGet(&allRenderObjects, i));
	}

	vectorClear(&allRenderObjects);
}