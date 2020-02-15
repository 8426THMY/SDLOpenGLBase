#include "guiElement.h"


#include "memoryManager.h"


void (*guiElementUpdateTable[GUI_ELEMENT_NUM_TYPES])(guiElement *const restrict gui, const float time) = {
	guiPanelUpdate,
	guiTextUpdate
};
void (*guiElementDrawTable[GUI_ELEMENT_NUM_TYPES])(
	const guiElement *const restrict gui, const shaderSprite *const restrict shader
) = {

	guiPanelDraw,
	guiTextDraw
};
void (*guiElementDeleteTable[GUI_ELEMENT_NUM_TYPES])(guiElement *const restrict gui) = {
	guiPanelDelete,
	guiTextDelete
};


// We need to initialise the element's data outside this function.
void guiElementInit(guiElement *const restrict gui, const guiElementType_t type){
	gui->type = type;

	transformStateInit(&gui->root);

	gui->parent = NULL;
	gui->children = NULL;
}


void guiElementUpdate(guiElement *const restrict gui, const float time){
	guiElementUpdateTable[gui->type](gui, time);
}

void guiElementDraw(
	guiElement *const restrict gui, const float windowWidth, const float windowHeight, const shaderSprite *const restrict shader
){

	mat4 viewProjectionMatrix;

	// We use a scale matrix as the view-projection matrix
	// so our interface's size can be specified in pixels.
	mat4InitScale(&viewProjectionMatrix, 2.f/windowWidth, 2.f/windowHeight, 1.f);
	glUniformMatrix4fv(shader->vpMatrixID, 1, GL_FALSE, (GLfloat *)&viewProjectionMatrix);

	guiElementDrawTable[gui->type](gui, shader);
}


void guiElementDelete(guiElement *const restrict gui){
	guiElementDeleteTable[gui->type](gui);
}