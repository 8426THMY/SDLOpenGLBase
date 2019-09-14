#include "renderable.h"


#define GLEW_STATIC
#include <GL/glew.h>


void renderableDefInit(renderableDef *renderDef, model *mdl){
	renderDef->mdl = mdl;
	renderDef->texGroup = mdl->texGroup;
}

void renderableInit(renderable *render, const renderableDef *renderDef){
	render->mdl = renderDef->mdl;
	texGroupStateInit(&render->texState, renderDef->texGroup);
}


// Update a renderable's current state.
void renderableUpdate(renderable *render, const float time){
	texGroupStateUpdate(&render->texState, time);
}

#warning "We probably shouldn't have the OpenGL drawing stuff split up so much."
void renderableDraw(const renderable *render, const shader *shaderProgram){
	// Bind the texture we're using!
	glBindTexture(GL_TEXTURE_2D, texGroupStateGetFrame(&render->texState, shaderProgram->uvOffsetsID));
	// Bind the vertex array object for the model!
	glBindVertexArray(render->mdl->vertexArrayID);

	// Draw the renderable!
	glDrawElements(GL_TRIANGLES, render->mdl->numIndices, GL_UNSIGNED_INT, 0);
}