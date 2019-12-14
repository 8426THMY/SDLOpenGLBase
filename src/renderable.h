#ifndef renderable_h
#define renderable_h


#include "vec3.h"
#include "mat4.h"

#include "model.h"
#include "textureGroup.h"
#include "skeleton.h"

#include "camera.h"
#include "shader.h"

#include "utilTypes.h"


#define BILLBOARD_DISABLED         0x00
// Stop the renderable from billboarding on certain axes.
#define BILLBOARD_LOCK_X           0x01
#define BILLBOARD_LOCK_Y           0x02
#define BILLBOARD_LOCK_Z           0x04
#define BILLBOARD_LOCK_XYZ         0x07
// Make the renderable keep a fixed scale regardless of distance.
#define BILLBOARD_SCALE            0x08
#define BILLBOARD_SPRITE           0x10
#define BILLBOARD_TARGET_SPRITE    0x20
#define BILLBOARD_TARGET_CAMERA    0x40
#define BILLBOARD_TARGET_ARBITRARY 0x80


typedef struct renderableDef {
	model *mdl;
	textureGroup *texGroup;
} renderableDef;

typedef struct renderable {
	model *mdl;
	textureGroupState texState;

	// Axis to rotate around.
	vec3 *axis;
	// Target position to look at.
	vec3 *target;

	flags_t flags;
} renderable;


void renderableDefInit(renderableDef *renderDef, model *mdl);
void renderableInit(renderable *render, const renderableDef *renderDef);

void renderableUpdate(renderable *render, const float time);
void renderableDraw(const renderable *render, const skeleton *objSkele, const mat4 *animStates, const shader *shaderPrg);

void renderableGenerateBillboardMatrix(
	const renderable *render, const camera *cam,
	const vec3 *centroid, const mat4 *root, mat4 *out
);


#endif