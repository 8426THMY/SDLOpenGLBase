#ifndef guiElement_h
#define guiElement_h


#include <stdint.h>

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


typedef uint_least8_t guiElementType_t;

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
	guiElementType_t type;

	// Stores the spatial configuration of the element.
	transformState root;

	guiElement *parent;
	#warning "Children are not currently implemented."
	guiElement *children;
} guiElement;


void guiElementInit(guiElement *const restrict gui, const guiElementType_t type);

void guiElementUpdate(guiElement *const restrict gui, const float time);
void guiElementDraw(
	guiElement *const restrict gui, const float windowWidth, const float windowHeight, const shaderSprite *const restrict shader
);

void guiElementDelete(guiElement *const restrict gui);


extern void (*guiElementUpdateTable[GUI_ELEMENT_NUM_TYPES])(
	guiElement *const restrict gui,
	const float time
);
extern void (*guiElementDrawTable[GUI_ELEMENT_NUM_TYPES])(
	const guiElement *const restrict gui,
	const shaderSprite *const restrict shader
);
extern void (*guiElementDeleteTable[GUI_ELEMENT_NUM_TYPES])(
	guiElement *const restrict gui
);


#endif