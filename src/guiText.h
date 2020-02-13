#ifndef guiText_h
#define guiText_h


#include "rectangle.h"
#include "transform.h"

#include "textFont.h"
#include "text.h"

#include "shader.h"


#define GUI_ELEMENT_TYPE_TEXT 1


typedef struct guiText {
	const textFont *font;
	textBuffer text;
	rectangle bounds;
} guiText;


void guiTextInit(guiText *const restrict gui, const textFont *const restrict font, const size_t bufferSize);

void guiTextUpdate(void *const restrict gui, const float time);
void guiTextDraw(
	const void *const restrict gui, const transformState *const restrict root, const shaderSprite *const restrict shader
);

void guiTextDelete(void *const restrict gui);


#endif