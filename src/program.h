#ifndef program_h
#define program_h


#define GLEW_STATIC
#include <GL/glew.h>

#include <SDL2/SDL.h>

#include "shader.h"
#include "camera.h"
#include "timestep.h"

#include "utilTypes.h"


typedef struct program {
	SDL_Window *window;
	int windowWidth;
	int windowHeight;

	int mouseX;
	int mouseY;
	const Uint8 *keyStates;

	return_t running;

	shaderObject objectShader;
	shaderSprite spriteShader;

	camera cam;

	timestep step;
} program;


return_t programInit(program *prg);
void programLoop(program *prg);
void programClose(program *prg);


#endif