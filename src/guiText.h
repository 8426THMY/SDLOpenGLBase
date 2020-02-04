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


void guiTextInit(guiText *gui, const textFont *font, const size_t bufferSize);

void guiTextUpdate(void *gui, const float time);
void guiTextDraw(const void *gui, const transformState *root, const shaderObject *shader);

void guiTextDelete(void *gui);


#endif