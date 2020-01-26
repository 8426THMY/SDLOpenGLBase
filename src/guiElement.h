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


/**
Ideas for panels:

Method:
	1. Store an array of four bones, one for each corner.
		Pros:
			- Borders can be kept a constant size by just resizing the center square.
			- Flexible animation.

		Cons:
			- Skeletal transformation can be slow.

	2. Draw using glBegin/glEnd.
		Pros:
			- Extremely easy.
			- Extremely flexible.

		Cons:
			- Slow.
			- SLOW!

	3. Render GUI elements to a texture whenever they change and draw them on a quad.
		Pros:
			- Extremely fast for static panels.

		Cons:
			- Possibly slow if the panels keep changing.
			- Fixed-sized borders might be difficult.

	4. Render separate quads for the panel and border components.
		Pros:
			- Fast for static and dynamic panels.
			- Handles fixed-sized borders very well.

		Cons:
			- Possibly not as flexible as other methods? Depends on how crazy you want to get.
**/

/**
Method 4 seems like the best. How will we store the textures, though?

Methods:
	1. Store nine textures, one for each border component plus the panel's background.
		Pros:
			- Easy to design panels.
			- Extremely flexible, components can be mixed and matched.
			- Stores the minimum amount of data for borders.
			- Good support for extremely high resolution backgrounds.
			- Extremely easy tiling.

		Cons:
			- Extremely high texture binding cost.
			- Extra textures may result in higher memory usage.

	2. Store two textures, one for the background and one for the entire border.
		Pros:
			- Fast.
			- Low draw cost.
			- Somewhat flexible, borders can be mixed and matched with backgrounds.
			- Good support for extremely high resolution backgrounds.
			- Easy tiling if the border is stored right.

		Cons:
			- Difficult to design panel borders.
			- Wastes a bit of memory if you want detailed edges but not very detailed corners.

	3. Store one texture that contains the background and border.
		Pros:
			- Extremely fast.
			- Extremely low draw cost.
			- Extremely easy to design panels.
			- Good support for extremely high resolution backgrounds.

		Cons:
			- Wastes a bit of memory for border edges.
			- Extremely inflexible.
			- Difficult tiling.
**/


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
	guiElement *children;
} guiElement;


void guiElementInit(guiElement *gui, const type_t type);

void guiElementUpdate(guiElement *gui, const float time);
void guiElementDraw(guiElement *gui, const float windowWidth, const float windowHeight, const shaderSprite *shader);

void guiElementDelete(guiElement *gui);


#endif