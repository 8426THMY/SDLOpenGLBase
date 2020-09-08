#include "guiElement.h"


#include "memoryManager.h"


void (*const guiElementUpdateTable[GUI_ELEMENT_NUM_TYPES])(guiElement *const restrict gui, const float time) = {
	guiPanelUpdate,
	guiTextUpdate
};
void (*const guiElementDrawTable[GUI_ELEMENT_NUM_TYPES])(
	const guiElement *const restrict gui, const spriteShader *const restrict shader
) = {

	guiPanelDraw,
	guiTextDraw
};
void (*const guiElementDeleteTable[GUI_ELEMENT_NUM_TYPES])(guiElement *const restrict gui) = {
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
	guiElement *const restrict gui, const int windowWidth, const int windowHeight, const spriteShader *const restrict shader
){

	mat4 viewProjectionMatrix;

	// We use a scale matrix as the view-projection matrix
	// so our interface's size can be specified in pixels.
	//
	// To help prevent texturing artifacts, we start from the middle of
	// the lower-left pixel and end at the middle of the upper-right pixel.
	#warning "We still get artifacts. Maybe use integer positions and scales for elements?"
	#warning "Also, it'd be nice if we could move this into our camera."
	//mat4Orthographic(&viewProjectionMatrix, (windowWidth - 1)*0.5f, -(windowWidth - 1)*0.5f, (windowHeight - 1)*0.5f, -(windowHeight - 1)*0.5f, -1000.f, 1000.f);
	mat4InitScale(&viewProjectionMatrix, 2.f/(windowWidth - 1), 2.f/(windowHeight - 1), -1.f/1000.f);
	glUniformMatrix4fv(shader->vpMatrixID, 1, GL_FALSE, (GLfloat *)&viewProjectionMatrix);

	guiElementDrawTable[gui->type](gui, shader);
}


void guiElementDelete(guiElement *const restrict gui){
	guiElementDeleteTable[gui->type](gui);
}