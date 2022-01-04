#ifndef guiElement_h
#define guiElement_h


#include <stdint.h>

#include "rectangle.h"
#include "mat4.h"
#include "transform.h"

#include "sprite.h"
#include "model.h"
#include "object.h"

#include "utilTypes.h"

#include "guiPanel.h"
#include "guiText.h"


#define GUI_ELEMENT_NUM_TYPES 2


/**#error "Work on GUIs a bit. How about a GUI animation system?"
#error "Maybe even better, let users specify an 'update' function for guiElements that manages the state machine."
#error "So it would handle any animations when creating or deleting it, as well as the creation and deletion of children."**/
typedef uint_least8_t guiElementType_t;

typedef struct guiElement guiElement;
typedef struct guiElement {
	// This should be large enough to
	// store any type of GUI element.
	union {
		guiPanel panel;
		model mdl;
		guiText text;
		object *obj;
	} data;
	// This specifies which
	// type of element it is.
	guiElementType_t type;

	// Stores the spatial configuration of the element.
	transform state;

	guiElement *parent;
	#warning "Children are not currently implemented."
	guiElement *children;
} guiElement;


void guiElementInit(guiElement *const restrict gui, const guiElementType_t type);

void guiElementUpdate(guiElement *const restrict gui, const float time);
void guiElementDraw(
	guiElement *const restrict gui, const int windowWidth, const int windowHeight, const spriteShader *const restrict shader
);

void guiElementDelete(guiElement *const restrict gui);

return_t guiElementSetup();
void guiElementCleanup();


extern void (*const guiElementUpdateTable[GUI_ELEMENT_NUM_TYPES])(
	guiElement *const restrict gui,
	const float time
);
extern void (*const guiElementDrawTable[GUI_ELEMENT_NUM_TYPES])(
	const guiElement *const restrict gui,
	const spriteShader *const restrict shader
);
extern void (*const guiElementDeleteTable[GUI_ELEMENT_NUM_TYPES])(
	guiElement *const restrict gui
);

extern sprite g_guiSpriteDefault;


#endif