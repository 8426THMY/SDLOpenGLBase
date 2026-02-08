#include "guiElement.h"


#define GLEW_STATIC
#include <GL/glew.h>

#include "mat4.h"

#include "memoryManager.h"


sprite g_guiSpriteDefault;


// We need to initialise the element's data outside this function.
void guiElementInit(guiElement *const restrict gui, const guiElementType type){
	gui->type = type;

	transformInit(&gui->state);

	gui->parent = NULL;
	gui->children = NULL;
}


void guiElementUpdate(guiElement *const restrict gui, const float dt){
	switch(gui->type){
		case GUI_ELEMENT_TYPE_PANEL:
			guiPanelUpdate(gui, dt);
		break;
		case GUI_ELEMENT_TYPE_TEXT:
			guiTextUpdate(gui, dt);
		break;
	}
}

void guiElementDraw(
	guiElement *const restrict gui,
	const int windowWidth, const int windowHeight,
	const spriteShader *const restrict shader
){

	mat4 vpMatrix;

	// We use a scale matrix as the view-projection matrix
	// so our interface's size can be specified in pixels.
	//
	// We treat the top-left corner of the screen as the origin, with the
	// x-axis increasing to the right and the y-axis increasing upwards.
	mat4Orthographic(&vpMatrix, (float)windowWidth, 0.f, 0.f, -(float)windowHeight, 0.f, 1.f);
	glUniformMatrix4fv(shader->vpMatrixID, 1, GL_FALSE, (GLfloat *)&vpMatrix);

	switch(gui->type){
		case GUI_ELEMENT_TYPE_PANEL:
			guiPanelDraw(gui, shader);
		break;
		case GUI_ELEMENT_TYPE_TEXT:
			guiTextDraw(gui, shader);
		break;
	}
}


void guiElementDelete(guiElement *const restrict gui){
	switch(gui->type){
		case GUI_ELEMENT_TYPE_PANEL:
			guiPanelDelete(gui);
		break;
		case GUI_ELEMENT_TYPE_TEXT:
			guiTextDelete(gui);
		break;
	}
}


return_t guiElementSetup(){
	const spriteVertex vertices[4] = {
		{
			.pos.x = 0.f, .pos.y =  0.f, .pos.z = 0.f,
			.uv.x = 0.f, .uv.y = 0.f
		},
		{
			.pos.x = 0.f, .pos.y = -1.f, .pos.z = 0.f,
			.uv.x = 0.f, .uv.y = 1.f
		},
		{
			.pos.x = 1.f, .pos.y = -1.f, .pos.z = 0.f,
			.uv.x = 1.f, .uv.y = 1.f
		},
		{
			.pos.x = 1.f, .pos.y =  0.f, .pos.z = 0.f,
			.uv.x = 1.f, .uv.y = 0.f
		}
	};

	const spriteVertexIndex indices[6] = {
		0, 1, 2,
		2, 3, 0
	};

	spriteGenerateBuffers(
		&g_guiSpriteDefault,
		vertices, sizeof(vertices)/sizeof(*vertices),
		indices, sizeof(indices)/sizeof(*indices)
	);


	return(1);
}

void guiElementCleanup(){
	spriteDelete(&g_guiSpriteDefault);
}