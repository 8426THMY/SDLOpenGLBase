#ifndef guiText_h
#define guiText_h


#include "sprite.h"
#include "textFont.h"
#include "text.h"


typedef struct guiText {
	textBuffer buffer;
	const textFont *font;

	// Bounds for the text box.
	float width;
	float height;
} guiText;

typedef struct guiElement guiElement;


void guiTextInit(guiText *const restrict gui, const textFont *const restrict font, const size_t bufferSize);

void guiTextUpdate(guiElement *const restrict gui, const float dt);
void guiTextDraw(
	const guiElement *const restrict gui, const spriteShader *const restrict shader
);

void guiTextDelete(guiElement *const restrict gui);


#endif