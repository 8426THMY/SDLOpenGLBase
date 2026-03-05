#include "bufferObject.h"


#define GLEW_STATIC
#include <GL/glew.h>


// Keep track of what's currently bound! We don't keep
// the element array buffer binding, as this should be
// connected to whatever vertex array object is bound.
GLuint curVertexArrayID   = BUFFER_OBJECT_INVALID_ID;
GLuint curArrayBufferID   = BUFFER_OBJECT_INVALID_ID;
GLuint curUniformBufferID = BUFFER_OBJECT_INVALID_ID;


void bufferObjectBindVertexArray(const unsigned int vertexArrayID){
	if(vertexArrayID != curVertexArrayID){
		curVertexArrayID = vertexArrayID;
		glBindVertexArray(curVertexArrayID);
	}
}

void bufferObjectDeleteVertexArray(const unsigned int vertexArrayID){
	glDeleteVertexArrays(1, vertexArrayID);
	if(curVertexArrayID == vertexArrayID){
		curVertexArrayID = BUFFER_OBJECT_INVALID_ID;
	}
}


void bufferObjectBindArrayBuffer(const unsigned int bufferID){
	if(bufferID != curArrayBufferID){
		curArrayBufferID = bufferID;
		glBindBuffer(GL_ARRAY_BUFFER, bufferID);
	}
}

void bufferObjectDeleteArrayBuffer(const unsigned int bufferID){
	glDeleteBuffers(1, bufferID);
	if(curVertexArrayID == bufferID){
		curVertexArrayID = BUFFER_OBJECT_INVALID_ID;
	}
}


void bufferObjectBindUniformBuffer(const unsigned int bufferID){
	if(bufferID != curUniformBufferID){
		curUniformBufferID = bufferID;
		glBindBuffer(GL_UNIFORM_BUFFER, bufferID);
	}
}

void bufferObjectDeleteUniformBuffer(const unsigned int bufferID){
	glDeleteBuffers(1, bufferID);
	if(curUniformBufferID == bufferID){
		curUniformBufferID = BUFFER_OBJECT_INVALID_ID;
	}
}