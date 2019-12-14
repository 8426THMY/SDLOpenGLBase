#include "guiElement.h"


#include "memoryManager.h"


void guiElementInit(guiElement *gui, const guiType_t type){
	guiPanelInit(&gui->data.panel);
	gui->type = type;

	gui->root = identityTransform;
	gui->root.scale.x = gui->root.scale.y = 100.f;

	gui->parent = NULL;
	gui->children = NULL;
}


void guiElementUpdate(guiElement *gui){
	//
}

void guiElementDraw(guiElement *gui, const float windowWidth, const float windowHeight, const shader *shaderPrg){
	mat4 viewProjection;
	//mat4 rootMatrix;

	// We use a scale matrix as the view-projection matrix
	// so our interface's size can be specified in pixels.
	mat4InitScale(&viewProjection, 2.f/windowWidth, 2.f/windowHeight, 1.f);
	glUniformMatrix4fv(shaderPrg->mvpMatrixID, 1, GL_FALSE, (GLfloat *)&viewProjection);

	//transformStateToMat4(&gui->root, &rootMatrix);
	guiPanelDraw(&gui->data.panel, &gui->root, shaderPrg);
}


void guiElementDelete(guiElement *gui){
	if(gui->children != NULL){
		memoryManagerGlobalFree(gui->children);
	}
}