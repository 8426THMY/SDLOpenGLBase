#ifndef guiText_h
#define guiText_h


#include "rectangle.h"
#include "transform.h"

#include "textFont.h"
#include "text.h"

#include "shader.h"


#define GUI_ELEMENT_TYPE_TEXT 1


typedef struct guiText {
	textBuffer buffer;
	const textFont *font;

	// Bounds for the text box.
	float width;
	float height;
} guiText;

typedef struct guiElement guiElement;


void guiTextInit(guiText *const restrict gui, const textFont *const restrict font, const size_t bufferSize);

void guiTextUpdate(guiElement *const restrict gui, const float time);
void guiTextDraw(
	const guiElement *const restrict gui, const shaderSprite *const restrict shader
);

void guiTextDelete(guiElement *const restrict gui);


#endif