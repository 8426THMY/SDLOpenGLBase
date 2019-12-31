#ifndef guiPanel_h
#define guiPanel_h


#include "vec2.h"
#include "transform.h"

#include "model.h"
#include "textureGroup.h"
#include "renderable.h"

#include "shader.h"

#include "utilTypes.h"


#define GUI_TYPE_PANEL 0

#define GUIPANEL_TILE_BORDER 0x01
#define GUIPANEL_TILE_BODY   0x02


typedef struct guiPanel {
	textureGroupState borderTexState;
	textureGroupState bodyTexState;

	// These are used for the UV coordinates of the
	// panel's border elements. They're ordered as follows:
	//
	// 0 - Bottom-right corner.
	// 1 - Top-right corner.
	// 2 - Top-left corner.
	// 3 - Bottom-left corner.
	// 4 - Right edge.
	// 5 - Top edge.
	// 6 - Left edge.
	// 7 - Bottom edge.
	rectangle uvCoords[8];

	flags_t flags;
} guiPanel;


void guiPanelInit(guiPanel *gui);

void guiPanelUpdate(guiPanel *gui, const float time);
void guiPanelDraw(const guiPanel *gui, const transformState *root, const shaderObject *shader);

/** TEMPORARY, USE SPRITES! **/
return_t guiPanelSetupDefault();


extern model guiPanelMdlDefault;


#endif