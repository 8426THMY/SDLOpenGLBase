#ifndef model_h
#define model_h


#include <stddef.h>

#include "vec2.h"
#include "vec3.h"

#include "mesh.h"
#include "skeleton.h"
#include "textureGroup.h"

#include "colliderAABB.h"
#include "collider.h"
#include "physicsRigidBody.h"

#include "renderFrustum.h"
#include "renderView.h"

#include "utilTypes.h"


#define MODEL_VERTEX_MAX_BONE_WEIGHTS 4


typedef meshVertexIndex modelVertexIndex;

typedef struct modelVertex {
	vec3 pos;
	vec2 uv;
	vec3 normal;

	boneIndex boneIDs[MODEL_VERTEX_MAX_BONE_WEIGHTS];
	float boneWeights[MODEL_VERTEX_MAX_BONE_WEIGHTS];
} modelVertex;


/*
** A model contains all the data pertaining to a particular
** animated mesh. This includes the meshes and textures, as
** well as the skeleton, colliders and rigid bodies.
**/
typedef struct modelDef {
	char *name;

	// Models use an individual mesh
	// for each separate texture group.
	#warning "It would be cool if we supported something like Source's bodygroups."
	mesh *meshes;
	// This is an array of texture group pointers, one per mesh.
	// The texture groups are stored in their respective allocator.
	#warning "Maybe we should have texture groups in the same sense as the Source engine?"
	textureGroup **texGroups;
	size_t numMeshes;

	skeleton *skele;

	// Array of colliders used for object collision.
	// A bone can have multiple colliders attached to it.
	collider *colliders;
	// Array of identifiers that tell us which bones
	// each collider should be attached to. This should
	// be sorted in order of increasing bone IDs.
	boneIndex *colliderBoneIDs;
	size_t numColliders;

	// Singly linked list of rigid body bases.
	// A bone can have at most one rigid body.
	#warning "We probably want to support joints, too."
	physicsRigidBodyDef *physBodies;
	// Array of identifiers that tell us which bones
	// each rigid body should be attached to. This should
	// be sorted in order of increasing bone IDs.
	boneIndex *physBoneIDs;
	size_t numBodies;

	// This bounding box is used for frustum culling.
	colliderAABB aabb;
} modelDef;

typedef struct model {
	const modelDef *mdlDef;

	// The size of this array should
	// always be equal to "mdlDef->numMeshes".
	textureGroupState *texStates;

	// Controls the object's position, orientation and scale.
	// This transformation is applied to the root bone during animation.
	#warning "This is a terrible way of doing it. The root bone should have its own dedicated transform, and this state should control everything."
	#warning "We should let whatever owns the model control this."
	boneState state;
	// Array of custom transformations to apply to each bone.
	// This can be used by physics or to simply move the object.
	boneState *boneTransforms;
	// Stores the skeleton and animations that this object is using.
	#warning "We shouldn't have skeleton states store another skeleton pointer."
	#warning "Instead, we should have animations store lookups into the model's skeleton."
	skeletonState skeleState;

	// Array of colliders stored in global space.
	collider *colliders;

	// Doubly linked list of rigid bodies. This is
	// usually inserted into a physics island's list.
	#warning "We probably want to support joints, too."
	physicsRigidBody *physBodies;

	// This bounding box is still used for frustum culling.
	colliderAABB aabb;
} model;


void modelInit(model *const restrict mdl, const modelDef *const restrict mdlDef);

#warning "The '.c' file is a bit messy. It would be nice to move the loading code out."
modelDef *modelDefOBJLoad(const char *const restrict mdlDefPath, const size_t mdlDefPathLength);
modelDef *modelDefSMDLoad(const char *const restrict mdlDefPath, const size_t mdlDefPathLength);

void modelUpdate(model *const restrict mdl, const float dt);

return_t modelInFrustum(
	const model *const restrict mdl,
	const renderFrustum *const restrict frustum
);
renderQueueID modelGetRenderQueueKey(
	const model *const restrict mdl,
	renderQueueKey *const restrict key
);
void modelUpdateGlobalTransform(
	model *const restrict mdl, const float dt
);
void modelPreDraw(
	const model *const restrict mdl,
	renderView *const restrict view
);
void modelDraw(const model *const restrict mdl);

void modelDelete(model *const restrict mdl);
void modelDefDelete(modelDef *const restrict mdlDef);

return_t modelSetup();
void modelCleanup();


extern modelDef g_mdlDefDefault;


#endif