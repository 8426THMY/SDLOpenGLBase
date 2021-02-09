#include "guiPanel.h"


#include <math.h>

#define GLEW_STATIC
#include <GL/glew.h>

#include "mat4.h"
#include "transform.h"

#include "texture.h"
#include "textureGroup.h"

#include "guiElement.h"


void guiPanelInit(guiPanel *const restrict gui){
	texGroupStateInit(&gui->borderTexState, &g_texGroupDefault);
	texGroupStateInit(&gui->bodyTexState, &g_texGroupDefault);

	// Bottom-right corner.
	gui->uvCoords[0].x = 0.f;
	gui->uvCoords[0].y = 0.f;
	gui->uvCoords[0].w = 1.f;
	gui->uvCoords[0].h = 1.f;
	// Top-right corner.
	gui->uvCoords[1] =
	// Top-left corner.
	gui->uvCoords[2] =
	// Bottom-left corner.
	gui->uvCoords[3] =

	// Right edge.
	gui->uvCoords[4] =
	// Top edge.
	gui->uvCoords[5] =
	// Left edge.
	gui->uvCoords[6] =
	// Bottom edge.
	gui->uvCoords[7] = gui->uvCoords[0];

	gui->flags = 0;
}


void guiPanelUpdate(guiElement *const restrict gui, const float time){
	texGroupStateUpdate(&(gui->data.panel.borderTexState), time);
	texGroupStateUpdate(&(gui->data.panel.bodyTexState), time);
}

void guiPanelDraw(
	const guiElement *const restrict gui, const spriteShader *const restrict shader
){

	spriteState panelStates[8];
	const textureGroupFrame *const borderFrame = texGroupStateGetFrame(&gui->data.panel.borderTexState);
	const textureGroupFrame *const bodyFrame = texGroupStateGetFrame(&gui->data.panel.bodyTexState);

	const rectangle *offsets = gui->data.panel.uvCoords;
	const float cornerPos[2] = {
		roundf(gui->root.pos.x),
		roundf(gui->root.pos.y)
	};
	const float bodyPos[2] = {
		cornerPos[0] + roundf(offsets[2].w * borderFrame->tex->width),
		cornerPos[1] - roundf(offsets[2].h * borderFrame->tex->height)
	};
	const float bodyWidth  = roundf(gui->root.scale.x);
	const float bodyHeight = roundf(gui->root.scale.y);
	float tempHeight;


	// Bind the mesh we're using!
	glBindVertexArray(g_guiSpriteDefault.vertexArrayID);


	glActiveTexture(GL_TEXTURE0);
	// Bind the border's texture!
	glBindTexture(GL_TEXTURE_2D, borderFrame->tex->id);


	#warning "Border tiling is not yet implemented."
	#warning "Neither is panel rotation."
	// Set up the bottom-right corner's state.
	mat4InitScale(&panelStates[0].state,
		roundf(offsets[0].w * borderFrame->tex->width), roundf(offsets[0].h * borderFrame->tex->height), 1.f
	);
	mat4TranslateTransform(&panelStates[0].state, bodyPos[0] + bodyWidth, bodyPos[1] - bodyHeight, 0.f);

	panelStates[0].uvOffsets = offsets[0];


	// Set up the top-right corner's state.
	mat4InitScale(&panelStates[1].state,
		roundf(offsets[1].w * borderFrame->tex->width), roundf(offsets[1].h * borderFrame->tex->height), 1.f
	);
	mat4TranslateTransform(&panelStates[1].state, bodyPos[0] + bodyWidth, cornerPos[1], 0.f);

	panelStates[1].uvOffsets = offsets[1];


	// Set up the top-left corner's state.
	mat4InitScale(&panelStates[2].state, bodyPos[0] - cornerPos[0], cornerPos[1] - bodyPos[1], 1.f);
	mat4TranslateTransform(&panelStates[2].state, cornerPos[0], cornerPos[1], 0.f);

	panelStates[2].uvOffsets = offsets[2];


	// Set up the bottom-left corner's state.
	mat4InitScale(&panelStates[3].state,
		roundf(offsets[3].w * borderFrame->tex->width), roundf(offsets[3].h * borderFrame->tex->height), 1.f
	);
	mat4TranslateTransform(&panelStates[3].state, cornerPos[0], bodyPos[1] - bodyHeight, 0.f);

	panelStates[3].uvOffsets = offsets[3];


	// Set up the right edge's state.
	tempHeight = roundf(offsets[4].h * borderFrame->tex->height);
	mat4InitScale(&panelStates[4].state, bodyHeight, tempHeight, 1.f);
	mat4RotateZ(&panelStates[4].state, -M_PI_2);
	mat4TranslateTransform(&panelStates[4].state, bodyPos[0] + bodyWidth + tempHeight, bodyPos[1], 0.f);

	panelStates[4].uvOffsets = offsets[4];


	// Set up the top edge's state.
	mat4InitScale(&panelStates[5].state, bodyWidth, roundf(offsets[5].h * borderFrame->tex->height), 1.f);
	mat4TranslateTransform(&panelStates[5].state, bodyPos[0], cornerPos[1], 0.f);

	panelStates[5].uvOffsets = offsets[5];


	// Set up the left edge's state.
	mat4InitScale(&panelStates[6].state, bodyHeight, roundf(offsets[6].h * borderFrame->tex->height), 1.f);
	mat4RotateZ(&panelStates[6].state, M_PI_2);
	mat4TranslateTransform(&panelStates[6].state, cornerPos[0], bodyPos[1] - bodyHeight, 0.f);

	panelStates[6].uvOffsets = offsets[6];


	// Set up the bottom edge's state.
	tempHeight = roundf(offsets[7].h * borderFrame->tex->height);
	mat4InitScale(&panelStates[7].state, bodyWidth, tempHeight, 1.f);
	mat4RotateZ(&panelStates[7].state, M_PI);
	mat4TranslateTransform(&panelStates[7].state, bodyPos[0] + bodyWidth, bodyPos[1] - bodyHeight - tempHeight, 0.f);

	panelStates[7].uvOffsets = offsets[7];


	// Upload the border elements' states to the shader.
	glBindBuffer(GL_ARRAY_BUFFER, g_guiSpriteDefault.stateBufferID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(panelStates), &panelStates[0]);
	// Draw each instance of the particle!
	glDrawElementsInstanced(GL_TRIANGLES, g_guiSpriteDefault.numIndices, GL_UNSIGNED_INT, NULL, 8);


	// Bind the body's texture!
	glBindTexture(GL_TEXTURE_2D, bodyFrame->tex->id);

	// Draw the main region of the panel.
	mat4InitScale(&panelStates[0].state, bodyWidth, bodyHeight, 1.f);
	mat4TranslateTransform(&panelStates[0].state, bodyPos[0], bodyPos[1], 0.f);

	panelStates[0].uvOffsets.x = bodyFrame->bounds.x;
	panelStates[0].uvOffsets.y = bodyFrame->bounds.y;
	// Scroll the body image if tiling is enabled.
	// Otherwise, we stretch it to fit the panel.
	if(flagsAreSet(gui->data.panel.flags, GUI_PANEL_TILE_BODY)){
		panelStates[0].uvOffsets.w = bodyWidth / bodyFrame->tex->width;
		panelStates[0].uvOffsets.h = bodyHeight / bodyFrame->tex->height;
	}else{
		panelStates[0].uvOffsets.w = bodyFrame->bounds.w;
		panelStates[0].uvOffsets.h = bodyFrame->bounds.h;
	}

	// Upload the body's state to the shader.
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(panelStates), &panelStates[0]);
	// Draw the panel's body!
	glDrawElements(GL_TRIANGLES, g_guiSpriteDefault.numIndices, GL_UNSIGNED_INT, 0);
}


void guiPanelDelete(guiElement *const restrict gui){
	return;
}