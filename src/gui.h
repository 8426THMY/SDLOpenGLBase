#ifndef gui_h
#define gui_h


#include "guiElement.h"


/*
** Stores all of the GUI elements for a GUI tree
** as well as information about the GUI, such as
** the screen dimensions.
*/
typedef struct gui {
	unsigned int windowWidth;
	unsigned int windowHeight;

	guiElement *elements;
	size_t numElements;
} gui;


#endif