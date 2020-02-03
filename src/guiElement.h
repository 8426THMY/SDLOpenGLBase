#ifndef guiElement_h
#define guiElement_h


#include "rectangle.h"
#include "mat4.h"
#include "transform.h"

#include "renderable.h"
#include "object.h"

#include "shader.h"

#include "utilTypes.h"

#include "guiPanel.h"
#include "guiText.h"


#define GUI_ELEMENT_NUM_TYPES 2


typedef struct guiElement guiElement;
typedef struct guiElement {
	// This should be large enough to
	// store any type of GUI element.
	union {
		guiPanel panel;
		renderable render;
		guiText text;
		object *obj;
	} data;
	// This specifies which
	// type of element it is.
	type_t type;

	// Stores the spatial configuration of the element.
	transformState root;

	guiElement *parent;
	#warning "Children are not currently implemented."
	guiElement *children;
} guiElement;


void guiElementInit(guiElement *gui, const type_t type);

void guiElementUpdate(guiElement *gui, const float time);
void guiElementDraw(guiElement *gui, const float windowWidth, const float windowHeight, const shaderSprite *shader);

void guiElementDelete(guiElement *gui);


extern void (*guiElementUpdateTable[GUI_ELEMENT_NUM_TYPES])(
	void *gui,
	const float time
);
extern void (*guiElementDrawTable[GUI_ELEMENT_NUM_TYPES])(
	const void *gui,
	const transformState *root,
	const shaderSprite *shader
);
extern void (*guiElementDeleteTable[GUI_ELEMENT_NUM_TYPES])(
	void *gui
);


#endif