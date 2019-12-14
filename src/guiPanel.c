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
	// Bottom-right corner.
	gui->uvCoords[0].x =
	gui->uvCoords[0].y = 0.f;
	gui->uvCoords[0].w =
	gui->uvCoords[0].h = 100.f;
	// Top-right corner.
	gui->uvCoords[1] =
	// Top-left corner.
	gui->uvCoords[2] =
	// Bottom-left corner.
	gui->uvCoords[3] = gui->uvCoords[0];

	// Bottom edge.
	gui->uvCoords[4].x =
	gui->uvCoords[4].y = 0.f;
	gui->uvCoords[4].w =
	gui->uvCoords[4].h = 100.f;
	// Right edge.
	gui->uvCoords[5] =
	// Top edge.
	gui->uvCoords[6] =
	// Left edge.
	gui->uvCoords[7] = gui->uvCoords[4];

	gui->flags = 0;
}


void guiPanelUpdate(guiPanel *gui){
	//
}

void guiPanelDraw(const guiPanel *gui, const transformState *root, const shader *shaderPrg){
	mat4 curState;
	const float uvOffsets[4] = {0.f, 0.f, 1.f, 1.f};


	glActiveTexture(GL_TEXTURE0);


	// Bind the vertex array object for the model!
	glBindVertexArray(guiPanelMdlDefault.vertexArrayID);
	// Bind the texture we're using!
	glBindTexture(GL_TEXTURE_2D, texDefault.id);


	// Draw the bottom-right corner of the border.
	curState.m[0][0] = gui->uvCoords[0].w;
	curState.m[0][1] = 0.f;
	curState.m[0][2] = 0.f;
	curState.m[0][3] = 0.f;

	curState.m[1][0] = 0.f;
	curState.m[1][1] = gui->uvCoords[0].h;
	curState.m[1][2] = 0.f;
	curState.m[1][3] = 0.f;

	curState.m[2][0] = 0.f;
	curState.m[2][1] = 0.f;
	curState.m[2][2] = 1.f;
	curState.m[2][3] = 0.f;

	curState.m[3][0] = root->pos.x + (root->scale.x + gui->uvCoords[0].w) * 0.5f;
	curState.m[3][1] = root->pos.y - (root->scale.y + gui->uvCoords[0].h) * 0.5f;
	curState.m[3][2] = 0.f;
	curState.m[3][3] = 1.f;

	glUniformMatrix4fv(shaderPrg->boneStatesID, 1, GL_FALSE, (GLfloat *)&curState);
	glUniform1fv(shaderPrg->uvOffsetsID, 4, (GLfloat *)&uvOffsets);
	glDrawElements(GL_TRIANGLES, guiPanelMdlDefault.numIndices, GL_UNSIGNED_INT, 0);


	// Draw the top-right corner of the border.
	curState.m[0][0] = gui->uvCoords[1].w;
	curState.m[0][1] = 0.f;
	curState.m[0][2] = 0.f;
	curState.m[0][3] = 0.f;

	curState.m[1][0] = 0.f;
	curState.m[1][1] = gui->uvCoords[1].h;
	curState.m[1][2] = 0.f;
	curState.m[1][3] = 0.f;

	curState.m[2][0] = 0.f;
	curState.m[2][1] = 0.f;
	curState.m[2][2] = 1.f;
	curState.m[2][3] = 0.f;

	curState.m[3][0] = root->pos.x + (root->scale.x + gui->uvCoords[1].w) * 0.5f;
	curState.m[3][1] = root->pos.y + (root->scale.y + gui->uvCoords[1].h) * 0.5f;
	curState.m[3][2] = 0.f;
	curState.m[3][3] = 1.f;

	glUniformMatrix4fv(shaderPrg->boneStatesID, 1, GL_FALSE, (GLfloat *)&curState);
	glUniform1fv(shaderPrg->uvOffsetsID, 4, (GLfloat *)&uvOffsets);
	glDrawElements(GL_TRIANGLES, guiPanelMdlDefault.numIndices, GL_UNSIGNED_INT, 0);


	// Draw the top-left corner of the border.
	curState.m[0][0] = gui->uvCoords[2].w;
	curState.m[0][1] = 0.f;
	curState.m[0][2] = 0.f;
	curState.m[0][3] = 0.f;

	curState.m[1][0] = 0.f;
	curState.m[1][1] = gui->uvCoords[2].h;
	curState.m[1][2] = 0.f;
	curState.m[1][3] = 0.f;

	curState.m[2][0] = 0.f;
	curState.m[2][1] = 0.f;
	curState.m[2][2] = 1.f;
	curState.m[2][3] = 0.f;

	curState.m[3][0] = root->pos.x - (root->scale.x + gui->uvCoords[2].w) * 0.5f;
	curState.m[3][1] = root->pos.y + (root->scale.y + gui->uvCoords[2].h) * 0.5f;
	curState.m[3][2] = 0.f;
	curState.m[3][3] = 1.f;

	glUniformMatrix4fv(shaderPrg->boneStatesID, 1, GL_FALSE, (GLfloat *)&curState);
	glUniform1fv(shaderPrg->uvOffsetsID, 4, (GLfloat *)&uvOffsets);
	glDrawElements(GL_TRIANGLES, guiPanelMdlDefault.numIndices, GL_UNSIGNED_INT, 0);


	// Draw the bottom-left corner of the border.
	curState.m[0][0] = gui->uvCoords[3].w;
	curState.m[0][1] = 0.f;
	curState.m[0][2] = 0.f;
	curState.m[0][3] = 0.f;

	curState.m[1][0] = 0.f;
	curState.m[1][1] = gui->uvCoords[3].h;
	curState.m[1][2] = 0.f;
	curState.m[1][3] = 0.f;

	curState.m[2][0] = 0.f;
	curState.m[2][1] = 0.f;
	curState.m[2][2] = 1.f;
	curState.m[2][3] = 0.f;

	curState.m[3][0] = root->pos.x - (root->scale.x + gui->uvCoords[3].w) * 0.5f;
	curState.m[3][1] = root->pos.y - (root->scale.y + gui->uvCoords[3].h) * 0.5f;
	curState.m[3][2] = 0.f;
	curState.m[3][3] = 1.f;

	glUniformMatrix4fv(shaderPrg->boneStatesID, 1, GL_FALSE, (GLfloat *)&curState);
	glUniform1fv(shaderPrg->uvOffsetsID, 4, (GLfloat *)&uvOffsets);
	glDrawElements(GL_TRIANGLES, guiPanelMdlDefault.numIndices, GL_UNSIGNED_INT, 0);


	// Draw the bottom edge of the border.
	curState.m[0][0] = -root->scale.x;
	curState.m[0][1] = 0.f;
	curState.m[0][2] = 0.f;
	curState.m[0][3] = 0.f;

	curState.m[1][0] = 0.f;
	curState.m[1][1] = -gui->uvCoords[4].h;
	curState.m[1][2] = 0.f;
	curState.m[1][3] = 0.f;

	curState.m[2][0] = 0.f;
	curState.m[2][1] = 0.f;
	curState.m[2][2] = 1.f;
	curState.m[2][3] = 0.f;

	curState.m[3][0] = root->pos.x;
	curState.m[3][1] = root->pos.y - (root->scale.y + gui->uvCoords[4].h) * 0.5f;
	curState.m[3][2] = 0.f;
	curState.m[3][3] = 1.f;

	glUniformMatrix4fv(shaderPrg->boneStatesID, 1, GL_FALSE, (GLfloat *)&curState);
	glUniform1fv(shaderPrg->uvOffsetsID, 4, (GLfloat *)&uvOffsets);
	glDrawElements(GL_TRIANGLES, guiPanelMdlDefault.numIndices, GL_UNSIGNED_INT, 0);


	// Draw the right edge of the border.
	curState.m[0][0] = 0.f;
	curState.m[0][1] = -root->scale.y;
	curState.m[0][2] = 0.f;
	curState.m[0][3] = 0.f;

	curState.m[1][0] = gui->uvCoords[4].w;
	curState.m[1][1] = 0.f;
	curState.m[1][2] = 0.f;
	curState.m[1][3] = 0.f;

	curState.m[2][0] = 0.f;
	curState.m[2][1] = 0.f;
	curState.m[2][2] = 1.f;
	curState.m[2][3] = 0.f;

	curState.m[3][0] = root->pos.x + (root->scale.x + gui->uvCoords[5].w) * 0.5f;
	curState.m[3][1] = root->pos.y;
	curState.m[3][2] = 0.f;
	curState.m[3][3] = 1.f;

	glUniformMatrix4fv(shaderPrg->boneStatesID, 1, GL_FALSE, (GLfloat *)&curState);
	glUniform1fv(shaderPrg->uvOffsetsID, 4, (GLfloat *)&uvOffsets);
	glDrawElements(GL_TRIANGLES, guiPanelMdlDefault.numIndices, GL_UNSIGNED_INT, 0);


	// Draw the top edge of the border.
	curState.m[0][0] = root->scale.x;
	curState.m[0][1] = 0.f;
	curState.m[0][2] = 0.f;
	curState.m[0][3] = 0.f;

	curState.m[1][0] = 0.f;
	curState.m[1][1] = gui->uvCoords[4].h;
	curState.m[1][2] = 0.f;
	curState.m[1][3] = 0.f;

	curState.m[2][0] = 0.f;
	curState.m[2][1] = 0.f;
	curState.m[2][2] = 1.f;
	curState.m[2][3] = 0.f;

	curState.m[3][0] = root->pos.x;
	curState.m[3][1] = root->pos.y + (root->scale.y + gui->uvCoords[6].h) * 0.5f;
	curState.m[3][2] = 0.f;
	curState.m[3][3] = 1.f;

	glUniformMatrix4fv(shaderPrg->boneStatesID, 1, GL_FALSE, (GLfloat *)&curState);
	glUniform1fv(shaderPrg->uvOffsetsID, 4, (GLfloat *)&uvOffsets);
	glDrawElements(GL_TRIANGLES, guiPanelMdlDefault.numIndices, GL_UNSIGNED_INT, 0);


	// Draw the left edge of the border.
	curState.m[0][0] = 0.f;
	curState.m[0][1] = root->scale.y;
	curState.m[0][2] = 0.f;
	curState.m[0][3] = 0.f;

	curState.m[1][0] = -gui->uvCoords[4].w;
	curState.m[1][1] = 0.f;
	curState.m[1][2] = 0.f;
	curState.m[1][3] = 0.f;

	curState.m[2][0] = 0.f;
	curState.m[2][1] = 0.f;
	curState.m[2][2] = 1.f;
	curState.m[2][3] = 0.f;

	curState.m[3][0] = root->pos.x - (root->scale.x + gui->uvCoords[5].w) * 0.5f;
	curState.m[3][1] = root->pos.y;
	curState.m[3][2] = 0.f;
	curState.m[3][3] = 1.f;

	glUniformMatrix4fv(shaderPrg->boneStatesID, 1, GL_FALSE, (GLfloat *)&curState);
	glUniform1fv(shaderPrg->uvOffsetsID, 4, (GLfloat *)&uvOffsets);
	glDrawElements(GL_TRIANGLES, guiPanelMdlDefault.numIndices, GL_UNSIGNED_INT, 0);


	// Draw the main region of the panel.
	transformStateToMat4(root, &curState);

	glUniformMatrix4fv(shaderPrg->boneStatesID, 1, GL_FALSE, (GLfloat *)&curState);
	glUniform1fv(shaderPrg->uvOffsetsID, 4, (GLfloat *)&uvOffsets);
	glDrawElements(GL_TRIANGLES, guiPanelMdlDefault.numIndices, GL_UNSIGNED_INT, 0);


	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}


/** TEMPORARY, USE SPRITES! **/
return_t guiPanelSetupDefault(){
	const vertex vertices[4] = {
		{
			.pos.x = -0.5f, .pos.y = 0.5f, .pos.z = 0.f,
			.uv.x = 0.f, .uv.y = 1.f,
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
			.uv.x = 1.f, .uv.y = 1.f,
			.normal.x = 0.f, .normal.y = 0.f, .normal.z = 1.f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		}
	};

	const size_t indices[6] = {
		0, 1, 2,
		2, 3, 0
	};


	guiPanelMdlDefault.numIndices = sizeof(indices)/sizeof(*indices);

	// Generate a vertex array object for our model and bind it!
	glGenVertexArrays(1, &guiPanelMdlDefault.vertexArrayID);
	glBindVertexArray(guiPanelMdlDefault.vertexArrayID);
		// Generate a vertex buffer object for our vertex data and bind it!
		glGenBuffers(1, &guiPanelMdlDefault.vertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, guiPanelMdlDefault.vertexBufferID);
		// Now add all our data to it!
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);

		// Generate a vertex buffer object for our indices and bind it!
		glGenBuffers(1, &guiPanelMdlDefault.indexBufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, guiPanelMdlDefault.indexBufferID);
		// Now add all our data to it!
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);


		// Models will need these three vertex attributes!
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);

		// Tell OpenGL which data belongs to the vertex attributes!
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*)offsetof(vertex, pos));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*)offsetof(vertex, uv));
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*)offsetof(vertex, normal));
		glVertexAttribIPointer(3, VERTEX_MAX_WEIGHTS, GL_UNSIGNED_INT, sizeof(vertex), (GLvoid *)offsetof(vertex, boneIDs));
		glVertexAttribPointer(4, VERTEX_MAX_WEIGHTS, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid *)offsetof(vertex, boneWeights));
	// Unbind the array object!
	glBindVertexArray(0);


	return(1);
}