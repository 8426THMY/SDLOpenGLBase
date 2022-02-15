#ifndef object_h
#define object_h


#include "utilTypes.h"

#include "camera.h"

#include "mesh.h"
#include "skeleton.h"
#include "model.h"
#include "collider.h"
#include "physicsRigidBody.h"


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


typedef struct objectDef {
	char *name;

	// Default skeleton for this object.
	skeleton *skele;

	// Array of colliders used for object collision.
	collider *colliders;

	// Singly linked list of rigid body bases.
	physicsRigidBodyDef *physBodies;
	// Array of identifiers that tell us which
	// bones each rigid body should be attached to.
	boneIndex_t *physBoneIDs;
	size_t numBodies;

	// Default array of drawable models
	// associated with this object.
	const modelDef **mdlDefs;
	size_t numModels;
} objectDef;

typedef struct object {
	const objectDef *objDef;

	// Controls the object's position, orientation and scale.
	// This transformation is applied to the root bone during animation.
	#warning "This is a terrible way of doing it. The root bone should have its own dedicated transform, and this state should control everything."
	boneState state;
	// Array of custom transformations to apply to each bone.
	// This can be used by physics or to simply move the object.
	boneState *boneTransforms;
	// Stores the skeleton and animations that this object is using.
	skeletonState skeleState;

	collider *colliders;

	// Doubly linked list of rigid bodies. This is usually inserted into an
	// island's list, so we need to know the total number of rigid bodies.
	// Note that these are stored in reverse order to the object definition!
	physicsRigidBody *physBodies;
	// Array of identifiers that tell us which
	// bones each rigid body should be attached to.
	boneIndex_t *physBoneIDs;
	size_t numBodies;

	// These models are drawn at the object's bones.
	model *mdls;
} object;


void objectDefInit(objectDef *const restrict objDef);
void objectInit(object *const restrict obj, const objectDef *const restrict objDef);

return_t objectDefLoad(objectDef *const restrict objDef, const char *const restrict objFile);

void objectSetSkeleton(object *const restrict obj, const skeleton *const restrict skele);
void objectAddRigidBody(
	object *const restrict obj,
	const physicsRigidBodyDef *const restrict bodyDef,
	const boneIndex_t boneID
);
void objectPreparePhysics(object *const restrict obj);

void objectUpdate(object *const restrict obj, const float time);
void objectDraw(
	const object *const restrict obj, const camera *const restrict cam,
	const meshShader *const restrict shader, const float time
);

void objectDelete(object *const restrict obj);
void objectDefDelete(objectDef *const restrict objDef);


#endif