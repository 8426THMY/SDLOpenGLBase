#ifndef guiText_h
#define guiText_h


#include "rectangle.h"
#include "mat4.h"

#include "shader.h"


#define GUI_TYPE_TEXT 1


typedef struct guiText {
	char *text;
	rectangle bounds;
} guiText;


void guiTextInit(guiText *gui);

void guiTextUpdate(guiText *gui);
void guiTextDraw(const guiText *gui, const mat4 *root, const shaderObject *shader);


#endif