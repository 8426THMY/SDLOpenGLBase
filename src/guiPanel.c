#include "guiPanel.h"


#include "sprite.h"
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

#warning "This is all very temporary. It'd be nice if we could fix the texture bleeding, though..."
void guiPanelDraw(
	const guiElement *const restrict gui, const shaderSprite *const restrict shader
){

	const guiPanel panel = gui->data.panel;

	spriteState panelStates[8];
	const textureGroupFrame *const borderFrame = texGroupStateGetFrame(&panel.borderTexState);
	const textureGroupFrame *const bodyFrame = texGroupStateGetFrame(&panel.bodyTexState);

	const rectangle *offsets = panel.uvCoords;
	float curWidth = offsets[0].w * borderFrame->diffuse->width;
	float curHeight = offsets[0].h * borderFrame->diffuse->height;
	const transformState rootTransform = {
		// Move the panel's origin to the top-left corner.
		.pos = {
			.x = gui->root.pos.x + gui->root.scale.x * 0.5f + curWidth,
			.y = gui->root.pos.y - gui->root.scale.y * 0.5f - curHeight,
			.z = gui->root.pos.z
		},
		.rot = gui->root.rot,
		.scale = gui->root.scale
	};


	// Bind the mesh we're using!
	glBindVertexArray(g_spriteDefault.vertexArrayID);


	glActiveTexture(GL_TEXTURE0);
	// Bind the border's texture!
	glBindTexture(GL_TEXTURE_2D, borderFrame->diffuse->id);


	#warning "Border tiling is not yet implemented."
	#warning "Neither is panel rotation."
	// Set up the bottom-right corner's state.
	panelStates[0].state.m[0][0] = curWidth;
	panelStates[0].state.m[0][1] = 0.f;
	panelStates[0].state.m[0][2] = 0.f;
	panelStates[0].state.m[0][3] = 0.f;

	panelStates[0].state.m[1][0] = 0.f;
	panelStates[0].state.m[1][1] = curHeight;
	panelStates[0].state.m[1][2] = 0.f;
	panelStates[0].state.m[1][3] = 0.f;

	panelStates[0].state.m[2][0] = 0.f;
	panelStates[0].state.m[2][1] = 0.f;
	panelStates[0].state.m[2][2] = 1.f;
	panelStates[0].state.m[2][3] = 0.f;

	panelStates[0].state.m[3][0] = rootTransform.pos.x + (rootTransform.scale.x + curWidth) * 0.5f;
	panelStates[0].state.m[3][1] = rootTransform.pos.y - (rootTransform.scale.y + curHeight) * 0.5f;
	panelStates[0].state.m[3][2] = 0.f;
	panelStates[0].state.m[3][3] = 1.f;

	panelStates[0].uvOffsets = offsets[0];


	// Set up the top-right corner's state.
	curWidth = offsets[1].w * borderFrame->diffuse->width;
	curHeight = offsets[1].h * borderFrame->diffuse->height;

	panelStates[1].state.m[0][0] = curWidth;
	panelStates[1].state.m[0][1] = 0.f;
	panelStates[1].state.m[0][2] = 0.f;
	panelStates[1].state.m[0][3] = 0.f;

	panelStates[1].state.m[1][0] = 0.f;
	panelStates[1].state.m[1][1] = curHeight;
	panelStates[1].state.m[1][2] = 0.f;
	panelStates[1].state.m[1][3] = 0.f;

	panelStates[1].state.m[2][0] = 0.f;
	panelStates[1].state.m[2][1] = 0.f;
	panelStates[1].state.m[2][2] = 1.f;
	panelStates[1].state.m[2][3] = 0.f;

	panelStates[1].state.m[3][0] = rootTransform.pos.x + (rootTransform.scale.x + curWidth) * 0.5f;
	panelStates[1].state.m[3][1] = rootTransform.pos.y + (rootTransform.scale.y + curHeight) * 0.5f;
	panelStates[1].state.m[3][2] = 0.f;
	panelStates[1].state.m[3][3] = 1.f;

	panelStates[1].uvOffsets = offsets[1];


	// Set up the top-left corner's state.
	curWidth = offsets[2].w * borderFrame->diffuse->width;
	curHeight = offsets[2].h * borderFrame->diffuse->height;

	panelStates[2].state.m[0][0] = curWidth;
	panelStates[2].state.m[0][1] = 0.f;
	panelStates[2].state.m[0][2] = 0.f;
	panelStates[2].state.m[0][3] = 0.f;

	panelStates[2].state.m[1][0] = 0.f;
	panelStates[2].state.m[1][1] = curHeight;
	panelStates[2].state.m[1][2] = 0.f;
	panelStates[2].state.m[1][3] = 0.f;

	panelStates[2].state.m[2][0] = 0.f;
	panelStates[2].state.m[2][1] = 0.f;
	panelStates[2].state.m[2][2] = 1.f;
	panelStates[2].state.m[2][3] = 0.f;

	panelStates[2].state.m[3][0] = rootTransform.pos.x - (rootTransform.scale.x + curWidth) * 0.5f;
	panelStates[2].state.m[3][1] = rootTransform.pos.y + (rootTransform.scale.y + curHeight) * 0.5f;
	panelStates[2].state.m[3][2] = 0.f;
	panelStates[2].state.m[3][3] = 1.f;

	panelStates[2].uvOffsets = offsets[2];


	// Set up the bottom-left corner's state.
	curWidth = offsets[3].w * borderFrame->diffuse->width;
	curHeight = offsets[3].h * borderFrame->diffuse->height;

	panelStates[3].state.m[0][0] = curWidth;
	panelStates[3].state.m[0][1] = 0.f;
	panelStates[3].state.m[0][2] = 0.f;
	panelStates[3].state.m[0][3] = 0.f;

	panelStates[3].state.m[1][0] = 0.f;
	panelStates[3].state.m[1][1] = curHeight;
	panelStates[3].state.m[1][2] = 0.f;
	panelStates[3].state.m[1][3] = 0.f;

	panelStates[3].state.m[2][0] = 0.f;
	panelStates[3].state.m[2][1] = 0.f;
	panelStates[3].state.m[2][2] = 1.f;
	panelStates[3].state.m[2][3] = 0.f;

	panelStates[3].state.m[3][0] = rootTransform.pos.x - (rootTransform.scale.x + curWidth) * 0.5f;
	panelStates[3].state.m[3][1] = rootTransform.pos.y - (rootTransform.scale.y + curHeight) * 0.5f;
	panelStates[3].state.m[3][2] = 0.f;
	panelStates[3].state.m[3][3] = 1.f;

	panelStates[3].uvOffsets = offsets[3];


	// Set up the right edge's state.
	curHeight = offsets[4].h * borderFrame->diffuse->height;

	panelStates[4].state.m[0][0] = 0.f;
	panelStates[4].state.m[0][1] = -rootTransform.scale.y;
	panelStates[4].state.m[0][2] = 0.f;
	panelStates[4].state.m[0][3] = 0.f;

	panelStates[4].state.m[1][0] = curHeight;
	panelStates[4].state.m[1][1] = 0.f;
	panelStates[4].state.m[1][2] = 0.f;
	panelStates[4].state.m[1][3] = 0.f;

	panelStates[4].state.m[2][0] = 0.f;
	panelStates[4].state.m[2][1] = 0.f;
	panelStates[4].state.m[2][2] = 1.f;
	panelStates[4].state.m[2][3] = 0.f;

	panelStates[4].state.m[3][0] = rootTransform.pos.x + (rootTransform.scale.x + curHeight) * 0.5f;
	panelStates[4].state.m[3][1] = rootTransform.pos.y;
	panelStates[4].state.m[3][2] = 0.f;
	panelStates[4].state.m[3][3] = 1.f;

	panelStates[4].uvOffsets = offsets[4];


	// Set up the top edge's state.
	curHeight = offsets[5].h * borderFrame->diffuse->height;

	panelStates[5].state.m[0][0] = rootTransform.scale.x;
	panelStates[5].state.m[0][1] = 0.f;
	panelStates[5].state.m[0][2] = 0.f;
	panelStates[5].state.m[0][3] = 0.f;

	panelStates[5].state.m[1][0] = 0.f;
	panelStates[5].state.m[1][1] = curHeight;
	panelStates[5].state.m[1][2] = 0.f;
	panelStates[5].state.m[1][3] = 0.f;

	panelStates[5].state.m[2][0] = 0.f;
	panelStates[5].state.m[2][1] = 0.f;
	panelStates[5].state.m[2][2] = 1.f;
	panelStates[5].state.m[2][3] = 0.f;

	panelStates[5].state.m[3][0] = rootTransform.pos.x;
	panelStates[5].state.m[3][1] = rootTransform.pos.y + (rootTransform.scale.y + curHeight) * 0.5f;
	panelStates[5].state.m[3][2] = 0.f;
	panelStates[5].state.m[3][3] = 1.f;

	panelStates[5].uvOffsets = offsets[5];


	// Set up the left edge's state.
	curHeight = offsets[6].h * borderFrame->diffuse->height;

	panelStates[6].state.m[0][0] = 0.f;
	panelStates[6].state.m[0][1] = rootTransform.scale.y;
	panelStates[6].state.m[0][2] = 0.f;
	panelStates[6].state.m[0][3] = 0.f;

	panelStates[6].state.m[1][0] = -curHeight;
	panelStates[6].state.m[1][1] = 0.f;
	panelStates[6].state.m[1][2] = 0.f;
	panelStates[6].state.m[1][3] = 0.f;

	panelStates[6].state.m[2][0] = 0.f;
	panelStates[6].state.m[2][1] = 0.f;
	panelStates[6].state.m[2][2] = 1.f;
	panelStates[6].state.m[2][3] = 0.f;

	panelStates[6].state.m[3][0] = rootTransform.pos.x - (rootTransform.scale.x + curHeight) * 0.5f;
	panelStates[6].state.m[3][1] = rootTransform.pos.y;
	panelStates[6].state.m[3][2] = 0.f;
	panelStates[6].state.m[3][3] = 1.f;

	panelStates[6].uvOffsets = offsets[6];


	// Set up the bottom edge's state.
	curHeight = offsets[7].h * borderFrame->diffuse->height;

	panelStates[7].state.m[0][0] = -rootTransform.scale.x;
	panelStates[7].state.m[0][1] = 0.f;
	panelStates[7].state.m[0][2] = 0.f;
	panelStates[7].state.m[0][3] = 0.f;

	panelStates[7].state.m[1][0] = 0.f;
	panelStates[7].state.m[1][1] = -curHeight;
	panelStates[7].state.m[1][2] = 0.f;
	panelStates[7].state.m[1][3] = 0.f;

	panelStates[7].state.m[2][0] = 0.f;
	panelStates[7].state.m[2][1] = 0.f;
	panelStates[7].state.m[2][2] = 1.f;
	panelStates[7].state.m[2][3] = 0.f;

	panelStates[7].state.m[3][0] = rootTransform.pos.x;
	panelStates[7].state.m[3][1] = rootTransform.pos.y - (rootTransform.scale.y + curHeight) * 0.5f;
	panelStates[7].state.m[3][2] = 0.f;
	panelStates[7].state.m[3][3] = 1.f;

	panelStates[7].uvOffsets = offsets[7];


	// Upload the border elements' states to the shader.
	glBindBuffer(GL_ARRAY_BUFFER, g_spriteDefault.stateBufferID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(panelStates), &panelStates[0]);
	// Draw each instance of the particle!
	glDrawElementsInstanced(GL_TRIANGLES, g_spriteDefault.numIndices, GL_UNSIGNED_INT, NULL, 8);


	// Bind the body's texture!
	glBindTexture(GL_TEXTURE_2D, bodyFrame->diffuse->id);

	// Draw the main region of the panel.
	transformStateToMat4(&rootTransform, &panelStates[0].state);

	panelStates[0].uvOffsets.x = bodyFrame->bounds.x;
	panelStates[0].uvOffsets.y = bodyFrame->bounds.y;
	// Scroll the body image if tiling is enabled.
	// Otherwise, we stretch it to fit the panel.
	if(flagsAreSet(panel.flags, GUI_PANEL_TILE_BODY)){
		panelStates[0].uvOffsets.w = rootTransform.scale.x / bodyFrame->diffuse->width;
		panelStates[0].uvOffsets.h = rootTransform.scale.y / bodyFrame->diffuse->height;
	}else{
		panelStates[0].uvOffsets.w = bodyFrame->bounds.w;
		panelStates[0].uvOffsets.h = bodyFrame->bounds.h;
	}

	// Upload the body's state to the shader.
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(panelStates), &panelStates[0]);
	// Draw the panel's body!
	glDrawElements(GL_TRIANGLES, g_spriteDefault.numIndices, GL_UNSIGNED_INT, 0);
}


void guiPanelDelete(guiElement *const restrict gui){
	return;
}