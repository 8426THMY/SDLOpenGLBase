#include "guiElement.h"


#define GLEW_STATIC
#include <GL/glew.h>

#include "memoryManager.h"


sprite g_guiSpriteDefault;


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

	transformRigidInit(&gui->root);

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
	// We treat the top-left corner of the screen as the origin, with the
	// x-axis increasing to the right and the y-axis increasing upwards.
	mat4Orthographic(&viewProjectionMatrix, (float)windowWidth, 0.f, 0.f, -(float)windowHeight, 0.f, 1.f);
	glUniformMatrix4fv(shader->vpMatrixID, 1, GL_FALSE, (GLfloat *)&viewProjectionMatrix);

	guiElementDrawTable[gui->type](gui, shader);
}


void guiElementDelete(guiElement *const restrict gui){
	guiElementDeleteTable[gui->type](gui);
}


return_t guiElementSetup(){
	const spriteVertex vertices[4] = {
		{
			.pos.x = 0.f, .pos.y =  0.f, .pos.z = 0.f,
			.uv.x = 0.f, 0.f
		},
		{
			.pos.x = 0.f, .pos.y = -1.f, .pos.z = 0.f,
			.uv.x = 0.f, 1.f
		},
		{
			.pos.x = 1.f, .pos.y = -1.f, .pos.z = 0.f,
			.uv.x = 1.f, 1.f
		},
		{
			.pos.x = 1.f, .pos.y =  0.f, .pos.z = 0.f,
			.uv.x = 1.f, 0.f
		}
	};

	const spriteVertexIndex_t indices[6] = {
		0, 1, 2,
		2, 3, 0
	};

	spriteGenerateBuffers(&g_guiSpriteDefault, vertices, sizeof(vertices)/sizeof(*vertices), indices, sizeof(indices)/sizeof(*indices));


	return(1);
}

void guiElementCleanup(){
	spriteDelete(&g_guiSpriteDefault);
}