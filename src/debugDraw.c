#include "debugDraw.h"


#define GLEW_STATIC
#include <GL/glew.h>


#include "shader.h"

#include "skeleton.h"
#include "colliderAABB.h"
#include "colliderHull.h"

#include "memoryManager.h"


typedef struct debugMesh {
	GLuint vertexArrayID;
	GLuint vertexBufferID;

	GLuint indexBufferID;
	size_t numIndices;

	GLuint drawMode;
} debugMesh;

typedef struct debugShader {
	GLuint programID;

	GLuint vpMatrixID;
	GLuint colourID;
} debugShader;


// We temporarily bind this shader when we're drawing debug data.
debugShader debugDrawShader;


// Forward-declare any helper functions!
static void debugMeshGenerateBuffers(
	debugMesh *const restrict meshData, const vec3 *const restrict vertices, const size_t numVertices,
	const size_t *const restrict indices, const size_t numIndices
);
static void debugMeshDrawBuffers(
	const debugMesh *const restrict meshData, const debugDrawInfo *const restrict info, const mat4 *const restrict vpMatrix
);
static void debugMeshDelete(const debugMesh *const restrict meshData);


debugDrawInfo debugDrawInfoInit(const unsigned int fillMode, const vec3 colour, const float size){
	const debugDrawInfo info = {
		.fillMode = fillMode,
		.colour = colour,
		.size = size
	};
	return(info);
}


return_t debugDrawSetup(){
	const GLuint vertexShaderID   = shaderLoad("./resource/shaders/debugVertexShader.gls",   GL_VERTEX_SHADER);
	const GLuint fragmentShaderID = shaderLoad("./resource/shaders/debugFragmentShader.gls", GL_FRAGMENT_SHADER);
	debugDrawShader.programID     = shaderLoadProgram(vertexShaderID, fragmentShaderID);

	shaderDelete(vertexShaderID);
	shaderDelete(fragmentShaderID);

	if(debugDrawShader.programID != SHADER_INVALID_ID){
		// Enable the shader we just loaded!
		glUseProgram(debugDrawShader.programID);
		// Find the positions of our shader's uniform variables!
		debugDrawShader.vpMatrixID = glGetUniformLocation(debugDrawShader.programID, "vpMatrix");
		debugDrawShader.colourID = glGetUniformLocation(debugDrawShader.programID, "meshColour");
		// Unbind the shader!
		glUseProgram(SHADER_INVALID_ID);

		return(1);
	}

	shaderDeleteProgram(debugDrawShader.programID);


	return(0);
}

// Draw a skeleton object using the settings supplied.
void debugDrawSkeleton(const skeletonObject *const restrict skeleData, const debugDrawInfo info, const mat4 *const restrict vpMatrix){
	debugMesh meshData;
	GLint prevArrayObject;
	GLint prevShader;


	vec3 *vertices;
	size_t *indices;
	const size_t numBones = skeleData->skele->numBones;
	size_t i = 0;

	vertices = memoryManagerGlobalAlloc(numBones * sizeof(*vertices));
	if(vertices == NULL){
		/** MALLOC FAILED **/
	}
	indices = memoryManagerGlobalAlloc(2*(numBones - 1) * sizeof(*indices));
	if(indices == NULL){
		/** MALLOC FAILED **/
	}
	meshData.numIndices = 0;


	// Copy each bone's position and create a new edge between it and its parent.
	for(; i < numBones; ++i){
		const size_t parentID = skeleData->skele->bones[i].parent;
		vertices[i] = skeleData->bones[i].pos;
		if(!valueIsInvalid(parentID, size_t)){
			indices[meshData.numIndices] = parentID;
			++meshData.numIndices;
			indices[meshData.numIndices] = i;
			++meshData.numIndices;
		}
	}


	// Make sure we keep the current global state so we can restore it after drawing.
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &prevArrayObject);
	glGetIntegerv(GL_CURRENT_PROGRAM, &prevShader);

	// Bind the debug shader for when we draw the mesh.
	glUseProgram(debugDrawShader.programID);
	// Generate temporary buffers for the mesh data.
	debugMeshGenerateBuffers(&meshData, vertices, numBones, indices, meshData.numIndices);
	// Draw the mesh from the buffers.
	meshData.drawMode = GL_LINES;
	debugMeshDrawBuffers(&meshData, &info, vpMatrix);
	// Now that we've drawn the mesh, we can destroy the buffers.
	debugMeshDelete(&meshData);

	// Restore the global state.
	glUseProgram(prevShader);
	glBindVertexArray(prevArrayObject);


	memoryManagerGlobalFree(indices);
	memoryManagerGlobalFree(vertices);
}

// Draw an axis-aligned bounding box using the settings supplied.
void debugDrawColliderAABB(const colliderAABB *aabb, const debugDrawInfo info, const mat4 *const restrict vpMatrix){
	debugMesh meshData;
	GLint prevArrayObject;
	GLint prevShader;


	const vec3 vertices[8] = {
		aabb->max,
		{.x = aabb->min.x, .y = aabb->max.y, .z = aabb->max.z},
		{.x = aabb->min.x, .y = aabb->max.y, .z = aabb->min.z},
		{.x = aabb->max.x, .y = aabb->max.y, .z = aabb->min.z},
		aabb->min,
		{.x = aabb->max.x, .y = aabb->min.y, .z = aabb->min.z},
		{.x = aabb->max.x, .y = aabb->min.y, .z = aabb->max.z},
		{.x = aabb->min.x, .y = aabb->min.y, .z = aabb->max.z}
	};
	const size_t indices[36] = {
		2, 1, 0,
		0, 3, 2,
		6, 0, 1,
		1, 7, 6,
		5, 3, 0,
		0, 6, 5,
		7, 1, 2,
		2, 4, 7,
		4, 2, 3,
		3, 5, 4,
		7, 4, 5,
		5, 6, 7
	};
	meshData.numIndices = 36;


	// Make sure we keep the current global state so we can restore it after drawing.
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &prevArrayObject);
	glGetIntegerv(GL_CURRENT_PROGRAM, &prevShader);

	// Bind the debug shader for when we draw the mesh.
	glUseProgram(debugDrawShader.programID);
	// Generate temporary buffers for the mesh data.
	debugMeshGenerateBuffers(&meshData, vertices, 8, indices, 36);
	// Draw the mesh from the buffers.
	meshData.drawMode = GL_TRIANGLES;
	debugMeshDrawBuffers(&meshData, &info, vpMatrix);
	// Now that we've drawn the mesh, we can destroy the buffers.
	debugMeshDelete(&meshData);

	// Restore the global state.
	glUseProgram(prevShader);
	glBindVertexArray(prevArrayObject);
}

// Draw a collider hull using the settings supplied.
void debugDrawColliderHull(const colliderHull *const restrict hull, const debugDrawInfo info, const mat4 *const restrict vpMatrix){
	debugMesh meshData;
	GLint prevArrayObject;
	GLint prevShader;


	// Compute an upper bound for the number of indices needed.
	// We assume that each face has the maximum number of edges.
	size_t *const indices = memoryManagerGlobalAlloc(hull->numFaces * (3 * (hull->maxFaceEdges - 2)) * sizeof(*indices));
	size_t *curIndex = indices;
	size_t curFaceIndex = 0;
	const colliderHullFace *curFace = hull->faces;
	meshData.numIndices = 0;

	if(indices == NULL){
		/** MALLOC FAILED **/
	}

	// Generate indices for each face in the hull.
	// Not all faces are triangles, so we may need to triangulate some.
	// However, we do assume they are all at least convex.
	for(; curFaceIndex < hull->numFaces; ++curFaceIndex, ++curFace){
		const colliderHullEdge *const startEdge = &hull->edges[*curFace];
		const colliderHullEdge *curEdge;

		colliderVertexIndex_t startIndex;
		if(curFaceIndex == startEdge->faceIndex){
			startIndex = startEdge->startVertexIndex;
			curEdge = &hull->edges[startEdge->nextIndex];

		// If the current face is the twin face for this edge, we'll need to use the ending vertex.
		}else{
			startIndex = startEdge->endVertexIndex;
			curEdge = &hull->edges[startEdge->twinNextIndex];
		}

		// Triangulate the face and generate indices using the triangle fan method.
		for(;;){
			// The order we add vertices in depends on whether
			// the current face is the twin face for this edge.
			if(curFaceIndex == curEdge->faceIndex){
				// If we've found the last edge, we're done!
				if(curEdge->endVertexIndex == startIndex){
					break;
				}

				// Add indices for this triangle.
				*curIndex = startIndex;
				++curIndex;
				*curIndex = curEdge->startVertexIndex;
				++curIndex;
				*curIndex = curEdge->endVertexIndex;
				++curIndex;
				meshData.numIndices += 3;

				// Continue to the next edge on the face.
				curEdge = &hull->edges[curEdge->nextIndex];

			// For twin faces, we really just add the
			// second and third vertices in opposite order.
			}else{
				if(curEdge->startVertexIndex == startIndex){
					break;
				}

				*curIndex = startIndex;
				++curIndex;
				*curIndex = curEdge->endVertexIndex;
				++curIndex;
				*curIndex = curEdge->startVertexIndex;
				++curIndex;
				meshData.numIndices += 3;

				curEdge = &hull->edges[curEdge->twinNextIndex];
			}
		}
	}


	// Make sure we keep the current global state so we can restore it after drawing.
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &prevArrayObject);
	glGetIntegerv(GL_CURRENT_PROGRAM, &prevShader);

	// Bind the debug shader for when we draw the mesh.
	glUseProgram(debugDrawShader.programID);
	// Generate temporary buffers for the mesh data.
	debugMeshGenerateBuffers(&meshData, hull->vertices, hull->numVertices, indices, meshData.numIndices);
	// Draw the mesh from the buffers.
	meshData.drawMode = GL_TRIANGLES;
	debugMeshDrawBuffers(&meshData, &info, vpMatrix);
	// Now that we've drawn the mesh, we can destroy the buffers.
	debugMeshDelete(&meshData);

	// Restore the global state.
	glUseProgram(prevShader);
	glBindVertexArray(prevArrayObject);


	memoryManagerGlobalFree(indices);
}

void debugDrawCleanup(){
	shaderDeleteProgram(debugDrawShader.programID);
}


// Generate vertex and index buffers to hold our mesh data!
static void debugMeshGenerateBuffers(
	debugMesh *const restrict meshData, const vec3 *const restrict vertices, const size_t numVertices,
	const size_t *const restrict indices, const size_t numIndices
){

	// Generate a vertex array object for our mesh and bind it!
	glGenVertexArrays(1, &meshData->vertexArrayID);
	glBindVertexArray(meshData->vertexArrayID);
		// Generate a buffer object for our vertex data and bind it!
		glGenBuffers(1, &meshData->vertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, meshData->vertexBufferID);
		// Now add all our data to it!
		glBufferData(GL_ARRAY_BUFFER, sizeof(*vertices) * numVertices, vertices, GL_STREAM_DRAW);

		// Set up the vertex array object attributes that require this buffer!
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(*vertices), 0);


		// Generate a buffer object for our indices and bind it!
		glGenBuffers(1, &meshData->indexBufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshData->indexBufferID);
		// Now add all our data to it!
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(*indices) * numIndices, indices, GL_STREAM_DRAW);
}

// Draw a debug mesh. This assumes the vertex array is bound.
static void debugMeshDrawBuffers(
	const debugMesh *const restrict meshData, const debugDrawInfo *const restrict info, const mat4 *const restrict vpMatrix
){

	// Disable culling so we can see the entire debug mesh.
	glDisable(GL_CULL_FACE);
	// Set the draw mode of the debug mesh.
	glPolygonMode(GL_FRONT_AND_BACK, info->fillMode);
	// Change the size of the debug mesh's features.
	switch(info->fillMode){
		case GL_LINE:
			glLineWidth(info->size);
		break;
		case GL_POINT:
			glPointSize(info->size);
		break;
	}

	glUniform3fv(debugDrawShader.colourID, 1, (GLfloat *)&info->colour);
	glUniformMatrix4fv(debugDrawShader.vpMatrixID, 1, GL_FALSE, (GLfloat *)vpMatrix);
	glDrawElements(meshData->drawMode, meshData->numIndices, GL_UNSIGNED_INT, NULL);

	// Restore the default settings.
	switch(info->fillMode){
		case GL_LINE:
			glLineWidth(1.f);
		break;
		case GL_POINT:
			glPointSize(1.f);
		break;
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	// Enable culling again, as everything else uses it (for now).
	glEnable(GL_CULL_FACE);
}

static void debugMeshDelete(const debugMesh *const restrict meshData){
	// This works because the buffer IDs are in consecutive memory,
	// though it does require our mesh structure to have no padding.
	glDeleteBuffers(2, &meshData->vertexBufferID);
	glDeleteVertexArrays(1, &meshData->vertexArrayID);
}