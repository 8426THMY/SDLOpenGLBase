#ifndef shaderSprite_h
#define shaderSprite_h


#define GLEW_STATIC
#include <GL/glew.h>

#include "utilTypes.h"


typedef struct shaderSprite {
	GLuint programID;

	GLuint vpMatrixID;
	GLuint sdfTypeID;
	GLuint sdfColourID;
	GLuint sdfBackgroundID;
} shaderSprite;


return_t shaderSpriteInit(shaderSprite *const restrict shader, const GLuint programID);


#endif