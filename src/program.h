#ifndef program_h
#define program_h


#define GLEW_STATIC
#include <GL/glew.h>

#include <SDL2/SDL.h>

#include "utilTypes.h"

#include "vec3.h"
#include "mat4.h"

#include "shader.h"
#include "fps.h"
#include "camera.h"


typedef struct program {
	SDL_Window *window;
	int windowWidth;
	int windowHeight;

	shader shaderProgram;

	return_t running;

	int mouseX;
	int mouseY;
	const Uint8 *keyStates;

	mat4 projectionMatrix;
	camera cam;

	fps framerate;
} program;


return_t programInit(program *prg);
void programLoop(program *prg);
void programClose(program *prg);


#endif