#include "guiElement.h"


#include "memoryManager.h"


void guiElementInit(guiElement *gui, const type_t type){
	guiPanelInit(&gui->data.panel);
	gui->type = type;

	transformStateInit(&gui->root);
	gui->root.scale.x = gui->root.scale.y = 100.f;

	gui->parent = NULL;
	gui->children = NULL;
}


void guiElementUpdate(guiElement *gui, const float time){
	guiPanelUpdate(&gui->data.panel, time);
}

void guiElementDraw(guiElement *gui, const float windowWidth, const float windowHeight, const shaderSprite *shader){
	mat4 viewProjectionMatrix;

	// We use a scale matrix as the view-projection matrix
	// so our interface's size can be specified in pixels.
	mat4InitScale(&viewProjectionMatrix, 2.f/windowWidth, 2.f/windowHeight, 1.f);
	glUniformMatrix4fv(shader->vpMatrixID, 1, GL_FALSE, (GLfloat *)&viewProjectionMatrix);

	guiPanelDraw(&gui->data.panel, &gui->root, shader);
}


void guiElementDelete(guiElement *gui){
	if(gui->children != NULL){
		memoryManagerGlobalFree(gui->children);
	}
}