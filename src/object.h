#ifndef object_h
#define object_h


#include "utilTypes.h"

#include "mat4.h"
#include "interp.h"
#include "shader.h"

#include "skeleton.h"
#include "collider.h"
#include "physicsRigidBody.h"
#include "renderable.h"


typedef struct objectDef {
	char *name;

	// Default skeleton for this object.
	skeleton *skele;

	// Arrays of colliders and rigid bodies for each bone.
	collider *colliders;
	physicsRigidBodyDef *physBodies;

	// Default array of drawable models
	// associated with this object.
	renderableDef *renderables;
} objectDef;

typedef struct object {
	const objectDef *objDef;

	// Stores the skeleton and animations that this object is using.
	skeletonObject skeleData;

	// Arrays of colliders and rigid bodies for each bone.
	// The colliders are for object collision, not rigid body collision.
	collider *colliders;
	physicsRigidBody *physBodies;

	// These models are drawn at the object's bones.
	renderable *renderables;
} object;


void objectDefInit(objectDef *objDef);
void objectInit(object *obj, const objectDef *objDef);

return_t objectDefLoad(objectDef *objDef, const char *objFile);

void objectUpdate(object *obj, const float time);
void objectDraw(const object *obj, mat4 viewProjectionMatrix, const shader *shaderPrg, const float time);

void objectDelete(object *obj);
void objectDefDelete(objectDef *objDef);


/**
Here's an idea (for the current object system):

1. Objects store an array of renderables. These are NOT attachements (like hats).
2. The renderables array just stores different parts of objects that may use different textures and should be treated separately (like eyes).
3. Objects are represented by entities, the next level up.
4. An entity would have an array of child entities, which may be objects, particle emitters, physics props, etcetera.
5. We can use the ideas below to attach them to the correct bones.
**/

/**
Here's an idea (for render objects):

1. Render objects store pivots. This says how they should be transformed.
2. When we draw a render object, its bones are positioned relative to the pivot.
3. Render objects draw themselves first, then all of their children.
4. Before drawing a child, it'll update the child's pivot to the state of the bone that child should be connected to.
5. Children are then rendered correctly and can be attached to any bone!
**/


#endif