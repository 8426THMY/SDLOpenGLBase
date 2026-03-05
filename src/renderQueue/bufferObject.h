#ifndef bufferObject_h
#define bufferObject_h


#define BUFFER_OBJECT_INVALID_ID 0


void bufferObjectBindVertexArray(const unsigned int vertexArrayID);
void bufferObjectDeleteVertexArray(const unsigned int vertexArrayID);

void bufferObjectBindArrayBuffer(const unsigned int bufferID);
void bufferObjectDeleteArrayBuffer(const unsigned int bufferID);

void bufferObjectBindUniformBuffer(const unsigned int bufferID);
void bufferObjectDeleteUniformBuffer(const unsigned int bufferID);


#endif