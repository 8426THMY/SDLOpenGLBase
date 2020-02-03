#include "guiElement.h"


#include "memoryManager.h"


void (*guiElementUpdateTable[GUI_ELEMENT_NUM_TYPES])(void *gui, const float time) = {
	guiPanelUpdate,
	guiTextUpdate
};
void (*guiElementDrawTable[GUI_ELEMENT_NUM_TYPES])(const void *gui, const transformState *root, const shaderSprite *shader) = {
	guiPanelDraw,
	guiTextDraw
};
void (*guiElementDeleteTable[GUI_ELEMENT_NUM_TYPES])(void *gui) = {
	guiPanelDelete,
	guiTextDelete
};


// We need to initialise the element's data outside this function.
void guiElementInit(guiElement *gui, const type_t type){
	gui->type = type;

	transformStateInit(&gui->root);
	gui->root.scale.x = gui->root.scale.y = 100.f;

	gui->parent = NULL;
	gui->children = NULL;
}


void guiElementUpdate(guiElement *gui, const float time){
	guiElementUpdateTable[gui->type]((void *)(&gui->data), time);
}

void guiElementDraw(guiElement *gui, const float windowWidth, const float windowHeight, const shaderSprite *shader){
	mat4 viewProjectionMatrix;
	const guiElement *curChild = gui->children;

	// We use a scale matrix as the view-projection matrix
	// so our interface's size can be specified in pixels.
	mat4InitScale(&viewProjectionMatrix, 2.f/windowWidth, 2.f/windowHeight, 1.f);
	glUniformMatrix4fv(shader->vpMatrixID, 1, GL_FALSE, (GLfloat *)&viewProjectionMatrix);

	guiElementDrawTable[gui->type]((void *)(&gui->data), &gui->root, shader);
}


void guiElementDelete(guiElement *gui){
	guiElementDeleteTable[gui->type]((void *)(&gui->data));
}