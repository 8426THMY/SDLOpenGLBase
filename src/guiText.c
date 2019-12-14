#include "guiText.h"


#include <stddef.h>


void guiTextInit(guiText *gui){
	gui->text = NULL;

	gui->bounds.x =
	gui->bounds.y =
	gui->bounds.w =
	gui->bounds.h = 0.f;
}


void guiTextUpdate(guiText *gui){
	//
}

void guiTextDraw(const guiText *gui, const mat4 *root, const shader *shaderPrg){
	//
}