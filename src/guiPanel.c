#include "guiPanel.h"


#include "vertex.h"

#include "texture.h"
#include "textureGroup.h"


model guiPanelMdlDefault = {
	.name     = "panel",
	.skele    = &skeleDefault,
	.texGroup = &texGroupDefault
};


void guiPanelInit(guiPanel *gui){
	texGroupStateInit(&gui->borderTexState, &texGroupDefault);
	texGroupStateInit(&gui->bodyTexState, &texGroupDefault);

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


void guiPanelUpdate(guiPanel *gui, const float time){
	//texGroupStateUpdate(&gui->borderTexState, time);
	texGroupStateUpdate(&gui->bodyTexState, time);
}

#warning "This is all very temporary. It'd be nice if we could fix the texture bleeding, though..."
void guiPanelDraw(const guiPanel *gui, const transformState *root, const shader *shaderPrg){
	mat4 curState;
	const textureGroupFrame *borderFrame = texGroupStateGetFrame(&gui->borderTexState);
	const textureGroupFrame *bodyFrame = texGroupStateGetFrame(&gui->bodyTexState);
	float curWidth;
	float curHeight;
	float bodyUVs[4] = {bodyFrame->bounds.x, bodyFrame->bounds.y, bodyFrame->bounds.w, bodyFrame->bounds.h};


	glActiveTexture(GL_TEXTURE0);
	// Bind the vertex array object for the panel!
	glBindVertexArray(guiPanelMdlDefault.meshData.vertexArrayID);


	// Bind the border's texture!
	glBindTexture(GL_TEXTURE_2D, borderFrame->diffuse->id);


	// Draw the bottom-right corner of the border.
	curWidth = gui->uvCoords[0].w * borderFrame->diffuse->width;
	curHeight = gui->uvCoords[0].h * borderFrame->diffuse->height;

	curState.m[0][0] = curWidth;
	curState.m[0][1] = 0.f;
	curState.m[0][2] = 0.f;
	curState.m[0][3] = 0.f;

	curState.m[1][0] = 0.f;
	curState.m[1][1] = curHeight;
	curState.m[1][2] = 0.f;
	curState.m[1][3] = 0.f;

	curState.m[2][0] = 0.f;
	curState.m[2][1] = 0.f;
	curState.m[2][2] = 1.f;
	curState.m[2][3] = 0.f;

	curState.m[3][0] = root->pos.x + (root->scale.x + curWidth) * 0.5f;
	curState.m[3][1] = root->pos.y - (root->scale.y + curHeight) * 0.5f;
	curState.m[3][2] = 0.f;
	curState.m[3][3] = 1.f;

	glUniformMatrix4fv(shaderPrg->boneStatesID, 1, GL_FALSE, (GLfloat *)&curState);
	glUniform1fv(shaderPrg->uvOffsetsID, 4, (GLfloat *)&gui->uvCoords[0]);
	glDrawElements(GL_TRIANGLES, guiPanelMdlDefault.meshData.numIndices, GL_UNSIGNED_INT, 0);


	// Draw the top-right corner of the border.
	curWidth = gui->uvCoords[1].w * borderFrame->diffuse->width;
	curHeight = gui->uvCoords[1].h * borderFrame->diffuse->height;

	curState.m[0][0] = curWidth;
	curState.m[0][1] = 0.f;
	curState.m[0][2] = 0.f;
	curState.m[0][3] = 0.f;

	curState.m[1][0] = 0.f;
	curState.m[1][1] = curHeight;
	curState.m[1][2] = 0.f;
	curState.m[1][3] = 0.f;

	curState.m[2][0] = 0.f;
	curState.m[2][1] = 0.f;
	curState.m[2][2] = 1.f;
	curState.m[2][3] = 0.f;

	curState.m[3][0] = root->pos.x + (root->scale.x + curWidth) * 0.5f;
	curState.m[3][1] = root->pos.y + (root->scale.y + curHeight) * 0.5f;
	curState.m[3][2] = 0.f;
	curState.m[3][3] = 1.f;

	glUniformMatrix4fv(shaderPrg->boneStatesID, 1, GL_FALSE, (GLfloat *)&curState);
	glUniform1fv(shaderPrg->uvOffsetsID, 4, (GLfloat *)&gui->uvCoords[1]);
	glDrawElements(GL_TRIANGLES, guiPanelMdlDefault.meshData.numIndices, GL_UNSIGNED_INT, 0);


	// Draw the top-left corner of the border.
	curWidth = gui->uvCoords[2].w * borderFrame->diffuse->width;
	curHeight = gui->uvCoords[2].h * borderFrame->diffuse->height;

	curState.m[0][0] = curWidth;
	curState.m[0][1] = 0.f;
	curState.m[0][2] = 0.f;
	curState.m[0][3] = 0.f;

	curState.m[1][0] = 0.f;
	curState.m[1][1] = curHeight;
	curState.m[1][2] = 0.f;
	curState.m[1][3] = 0.f;

	curState.m[2][0] = 0.f;
	curState.m[2][1] = 0.f;
	curState.m[2][2] = 1.f;
	curState.m[2][3] = 0.f;

	curState.m[3][0] = root->pos.x - (root->scale.x + curWidth) * 0.5f;
	curState.m[3][1] = root->pos.y + (root->scale.y + curHeight) * 0.5f;
	curState.m[3][2] = 0.f;
	curState.m[3][3] = 1.f;

	glUniformMatrix4fv(shaderPrg->boneStatesID, 1, GL_FALSE, (GLfloat *)&curState);
	glUniform1fv(shaderPrg->uvOffsetsID, 4, (GLfloat *)&gui->uvCoords[2]);
	glDrawElements(GL_TRIANGLES, guiPanelMdlDefault.meshData.numIndices, GL_UNSIGNED_INT, 0);


	// Draw the bottom-left corner of the border.
	curWidth = gui->uvCoords[3].w * borderFrame->diffuse->width;
	curHeight = gui->uvCoords[3].h * borderFrame->diffuse->height;

	curState.m[0][0] = curWidth;
	curState.m[0][1] = 0.f;
	curState.m[0][2] = 0.f;
	curState.m[0][3] = 0.f;

	curState.m[1][0] = 0.f;
	curState.m[1][1] = curHeight;
	curState.m[1][2] = 0.f;
	curState.m[1][3] = 0.f;

	curState.m[2][0] = 0.f;
	curState.m[2][1] = 0.f;
	curState.m[2][2] = 1.f;
	curState.m[2][3] = 0.f;

	curState.m[3][0] = root->pos.x - (root->scale.x + curWidth) * 0.5f;
	curState.m[3][1] = root->pos.y - (root->scale.y + curHeight) * 0.5f;
	curState.m[3][2] = 0.f;
	curState.m[3][3] = 1.f;

	glUniformMatrix4fv(shaderPrg->boneStatesID, 1, GL_FALSE, (GLfloat *)&curState);
	glUniform1fv(shaderPrg->uvOffsetsID, 4, (GLfloat *)&gui->uvCoords[3]);
	glDrawElements(GL_TRIANGLES, guiPanelMdlDefault.meshData.numIndices, GL_UNSIGNED_INT, 0);


	// Draw the right edge of the border.
	curHeight = gui->uvCoords[4].h * borderFrame->diffuse->height;

	curState.m[0][0] = 0.f;
	curState.m[0][1] = -root->scale.y;
	curState.m[0][2] = 0.f;
	curState.m[0][3] = 0.f;

	curState.m[1][0] = curHeight;
	curState.m[1][1] = 0.f;
	curState.m[1][2] = 0.f;
	curState.m[1][3] = 0.f;

	curState.m[2][0] = 0.f;
	curState.m[2][1] = 0.f;
	curState.m[2][2] = 1.f;
	curState.m[2][3] = 0.f;

	curState.m[3][0] = root->pos.x + (root->scale.x + curHeight) * 0.5f;
	curState.m[3][1] = root->pos.y;
	curState.m[3][2] = 0.f;
	curState.m[3][3] = 1.f;

	glUniformMatrix4fv(shaderPrg->boneStatesID, 1, GL_FALSE, (GLfloat *)&curState);
	glUniform1fv(shaderPrg->uvOffsetsID, 4, (GLfloat *)&gui->uvCoords[4]);
	glDrawElements(GL_TRIANGLES, guiPanelMdlDefault.meshData.numIndices, GL_UNSIGNED_INT, 0);


	// Draw the top edge of the border.
	curHeight = gui->uvCoords[5].h * borderFrame->diffuse->height;

	curState.m[0][0] = root->scale.x;
	curState.m[0][1] = 0.f;
	curState.m[0][2] = 0.f;
	curState.m[0][3] = 0.f;

	curState.m[1][0] = 0.f;
	curState.m[1][1] = curHeight;
	curState.m[1][2] = 0.f;
	curState.m[1][3] = 0.f;

	curState.m[2][0] = 0.f;
	curState.m[2][1] = 0.f;
	curState.m[2][2] = 1.f;
	curState.m[2][3] = 0.f;

	curState.m[3][0] = root->pos.x;
	curState.m[3][1] = root->pos.y + (root->scale.y + curHeight) * 0.5f;
	curState.m[3][2] = 0.f;
	curState.m[3][3] = 1.f;

	glUniformMatrix4fv(shaderPrg->boneStatesID, 1, GL_FALSE, (GLfloat *)&curState);
	glUniform1fv(shaderPrg->uvOffsetsID, 4, (GLfloat *)&gui->uvCoords[5]);
	glDrawElements(GL_TRIANGLES, guiPanelMdlDefault.meshData.numIndices, GL_UNSIGNED_INT, 0);


	// Draw the left edge of the border.
	curHeight = gui->uvCoords[6].h * borderFrame->diffuse->height;

	curState.m[0][0] = 0.f;
	curState.m[0][1] = root->scale.y;
	curState.m[0][2] = 0.f;
	curState.m[0][3] = 0.f;

	curState.m[1][0] = -curHeight;
	curState.m[1][1] = 0.f;
	curState.m[1][2] = 0.f;
	curState.m[1][3] = 0.f;

	curState.m[2][0] = 0.f;
	curState.m[2][1] = 0.f;
	curState.m[2][2] = 1.f;
	curState.m[2][3] = 0.f;

	curState.m[3][0] = root->pos.x - (root->scale.x + curHeight) * 0.5f;
	curState.m[3][1] = root->pos.y;
	curState.m[3][2] = 0.f;
	curState.m[3][3] = 1.f;

	glUniformMatrix4fv(shaderPrg->boneStatesID, 1, GL_FALSE, (GLfloat *)&curState);
	glUniform1fv(shaderPrg->uvOffsetsID, 4, (GLfloat *)&gui->uvCoords[6]);
	glDrawElements(GL_TRIANGLES, guiPanelMdlDefault.meshData.numIndices, GL_UNSIGNED_INT, 0);


	// Draw the bottom edge of the border.
	curHeight = gui->uvCoords[7].h * borderFrame->diffuse->height;

	curState.m[0][0] = -root->scale.x;
	curState.m[0][1] = 0.f;
	curState.m[0][2] = 0.f;
	curState.m[0][3] = 0.f;

	curState.m[1][0] = 0.f;
	curState.m[1][1] = -curHeight;
	curState.m[1][2] = 0.f;
	curState.m[1][3] = 0.f;

	curState.m[2][0] = 0.f;
	curState.m[2][1] = 0.f;
	curState.m[2][2] = 1.f;
	curState.m[2][3] = 0.f;

	curState.m[3][0] = root->pos.x;
	curState.m[3][1] = root->pos.y - (root->scale.y + curHeight) * 0.5f;
	curState.m[3][2] = 0.f;
	curState.m[3][3] = 1.f;

	glUniformMatrix4fv(shaderPrg->boneStatesID, 1, GL_FALSE, (GLfloat *)&curState);
	glUniform1fv(shaderPrg->uvOffsetsID, 4, (GLfloat *)&gui->uvCoords[7]);
	glDrawElements(GL_TRIANGLES, guiPanelMdlDefault.meshData.numIndices, GL_UNSIGNED_INT, 0);


	// Bind the body's texture!
	glBindTexture(GL_TEXTURE_2D, bodyFrame->diffuse->id);

	// Draw the main region of the panel.
	transformStateToMat4(root, &curState);

	// Scroll the body image if tiling is enabled.
	// Otherwise, we stretch it to fit the panel.
	if(flagsAreSet(gui->flags, GUIPANEL_TILE_BODY)){
		bodyUVs[2] = root->scale.x / bodyFrame->diffuse->width;
		bodyUVs[3] = root->scale.y / bodyFrame->diffuse->height;
	}

	glUniformMatrix4fv(shaderPrg->boneStatesID, 1, GL_FALSE, (GLfloat *)&curState);
	glUniform1fv(shaderPrg->uvOffsetsID, 4, (GLfloat *)&bodyUVs[0]);
	glDrawElements(GL_TRIANGLES, guiPanelMdlDefault.meshData.numIndices, GL_UNSIGNED_INT, 0);


	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}


/** TEMPORARY, USE SPRITES! **/
return_t guiPanelSetupDefault(){
	const vertex vertices[4] = {
		{
			.pos.x = -0.5f, .pos.y = 0.5f, .pos.z = 0.f,
			.uv.x = 0.f, .uv.y = -1.f,
			.normal.x = 0.f, .normal.y = 0.f, .normal.z = 1.f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		},
		{
			.pos.x = -0.5f, .pos.y = -0.5f, .pos.z = 0.f,
			.uv.x = 0.f, .uv.y = 0.f,
			.normal.x = 0.f, .normal.y = 0.f, .normal.z = 1.f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		},
		{
			.pos.x = 0.5f, .pos.y = -0.5f, .pos.z = 0.f,
			.uv.x = 1.f, .uv.y = 0.f,
			.normal.x = 0.f, .normal.y = 0.f, .normal.z = 1.f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		},
		{
			.pos.x = 0.5f, .pos.y = 0.5f, .pos.z = 0.f,
			.uv.x = 1.f, .uv.y = -1.f,
			.normal.x = 0.f, .normal.y = 0.f, .normal.z = 1.f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		}
	};

	const size_t indices[6] = {
		0, 1, 2,
		2, 3, 0
	};

	meshGenerateBuffers(&guiPanelMdlDefault.meshData, vertices, sizeof(vertices)/sizeof(*vertices), indices, sizeof(indices)/sizeof(*indices));


	return(1);
}