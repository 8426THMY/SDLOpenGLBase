#ifndef physicsRigidBody_h
#define physicsRigidBody_h


#include <stdint.h>

#include "vec3.h"
#include "quat.h"
#include "mat3.h"
#include "transform.h"

#include "physicsCollider.h"
#include "physicsJoint.h"

#include "utilTypes.h"


// If we're using non-linear Gauss-Seidel stabilisation
// anywhere, we'll need to define some additional functions.
#if defined(PHYSCONTACT_STABILISER_GAUSS_SEIDEL) || defined(PHYSJOINT_USE_POSITIONAL_CORRECTION)
	#define PHYSCOLLIDER_USE_POSITIONAL_CORRECTION
#endif

// The body should be simulated in some way.
#define PHYSRIGIDBODY_SIMULATE_LINEAR    0x01
#define PHYSRIGIDBODY_SIMULATE_ANGULAR   0x02
#define PHYSRIGIDBODY_SIMULATE           (PHYSRIGIDBODY_SIMULATE_LINEAR | PHYSRIGIDBODY_SIMULATE_ANGULAR)
// The body should allow collisions.
#define PHYSRIGIDBODY_COLLIDE            0x04
// The body has been transformed in some way.
#define PHYSRIGIDBODY_TRANSLATED         0x10
#define PHYSRIGIDBODY_ROTATED            0x20
#define PHYSRIGIDBODY_TRANSFORMED        0x40

#ifndef PHYSRIGIDBODY_DEFAULT_STATE
	#define PHYSRIGIDBODY_DEFAULT_STATE (PHYSRIGIDBODY_SIMULATE | PHYSRIGIDBODY_COLLIDE)
#endif

#define physRigidBodySimulateLinear(body) flagsSet(body->flags, PHYSRIGIDBODY_SIMULATE_LINEAR)
#define physRigidBodySimulateAngular(body) flagsSet(body->flags, PHYSRIGIDBODY_SIMULATE_ANGULAR)
#define physRigidBodySimulateCollisions(body) flagsSet(body->flags, PHYSRIGIDBODY_COLLIDE)

#define physRigidBodyIgnoreLinear(body) flagsUnset(body->flags, PHYSRIGIDBODY_SIMULATE_LINEAR)
#define physRigidBodyIgnoreAngular(body) flagsUnset(body->flags, PHYSRIGIDBODY_SIMULATE_ANGULAR)
#define physRigidBodyIgnoreCollisions(body) flagsUnset(body->flags, PHYSRIGIDBODY_COLLIDE)

#define physRigidBodyIsSimulated(body) flagsAreSet(body->flags, PHYSRIGIDBODY_SIMULATE)
#define physRigidBodyIsCollidable(body) flagsAreSet(body->flags, PHYSRIGIDBODY_COLLIDE)


typedef struct physicsRigidBodyDef {
	// Singly linked list of colliders used by this rigid body.
	physicsCollider *colliders;

	// The rigid body's physical properties.
	float mass;
	float invMass;

	// The local centroid of the body.
	vec3 centroid;
	// Matrix that describes how the body
	// resists rotation around an axis.
	mat3 inertia;

	// Default flags for the rigid body.
	flags_t flags;
} physicsRigidBodyDef;

#warning "Linear and angular damping would also be nice."
// Rigid body instance.
typedef struct physicsRigidBody {
	const physicsRigidBodyDef *base;

	// Singly linked list of colliders used by this rigid body.
	physicsCollider *colliders;

	float mass;
	float invMass;

	// The global centroid of the body.
	vec3 centroid;
	// We store the local inverse so we don't need to
	// recompute it every time for scaled rigid bodies.
	mat3 invInertiaLocal;
	// The inverse global inertia tensor of the body.
	mat3 invInertiaGlobal;

	// Defines the body's spacial configuration.
	// The position stored here should not be
	// confused with the rigid body's centroid!
	transformState state;

	// Defines the body's physical configuration.
	vec3 linearVelocity;
	vec3 angularVelocity;
	vec3 netForce;
	vec3 netTorque;

	// They also store a quad-list of joints
	// that they are a part of. These behave
	// similarly to the previous linked lists.
	physicsJoint *joints;

	flags_t flags;
} physicsRigidBody;


void physRigidBodyDefInit(physicsRigidBodyDef *const restrict bodyDef);
void physRigidBodyInit(physicsRigidBody *const restrict body, const physicsRigidBodyDef *const restrict bodyDef);

return_t physRigidBodyDefLoad(physicsRigidBodyDef **const restrict bodies, const char *const restrict bodyPath, const size_t bodyPathLength);

void physRigidBodyDefAddCollider(
	physicsRigidBodyDef *const restrict bodyDef, const float mass,
	const vec3 *const restrict centroid, mat3 inertia
);
#if 0
/** I don't think we need these anymore, as the function above handles this stuff more cleanly. **/
void physRigidBodyDefSumCentroids(physicsRigidBodyDef *const restrict bodyDef, const vec3 *centroidArray, const size_t numBodies);
void physRigidBodyDefSumInertia(physicsRigidBodyDef *const restrict bodyDef, const mat3 *inertiaArray, const size_t numBodies);
void physRigidBodyDefGenerateProperties(physicsRigidBodyDef *const restrict bodyDef, const float **const restrict massArrays);
#endif

void physRigidBodyIntegrateVelocity(physicsRigidBody *const restrict body, const float dt);
void physRigidBodyResetAccumulators(physicsRigidBody *const restrict body);
void physRigidBodyIntegratePosition(physicsRigidBody *const restrict body, const float dt);

void physRigidBodyApplyLinearForce(physicsRigidBody *const restrict body, const vec3 *const restrict F);
void physRigidBodyApplyAngularForce(physicsRigidBody *const restrict body, const vec3 *const restrict r, const vec3 *const restrict F);
void physRigidBodyApplyForce(physicsRigidBody *const restrict body, const vec3 *const restrict r, const vec3 *const restrict F);

void physRigidBodyApplyLinearImpulse(physicsRigidBody *const restrict body, vec3 J);
void physRigidBodyApplyLinearImpulseInverse(physicsRigidBody *const restrict body, vec3 J);
void physRigidBodyApplyAngularImpulse(physicsRigidBody *const restrict body, vec3 J);
void physRigidBodyApplyAngularImpulseInverse(physicsRigidBody *const restrict body, vec3 J);
void physRigidBodyApplyImpulse(physicsRigidBody *const restrict body, const vec3 *const restrict r, const vec3 *const restrict p);
void physRigidBodyApplyImpulseBoost(physicsRigidBody *const restrict body, const vec3 *const restrict r, const vec3 *const restrict p, const vec3 *const restrict a);
void physRigidBodyApplyImpulseInverse(physicsRigidBody *const restrict body, const vec3 *const restrict r, const vec3 *const restrict p);
void physRigidBodyApplyImpulseBoostInverse(physicsRigidBody *const restrict body, const vec3 *const restrict r, const vec3 *const restrict p, const vec3 *const restrict a);
#ifdef PHYSCOLLIDER_USE_POSITIONAL_CORRECTION
void physRigidBodyApplyImpulsePosition(physicsRigidBody *const restrict body, const vec3 *const restrict r, const vec3 *const restrict p);
void physRigidBodyApplyImpulsePositionInverse(physicsRigidBody *const restrict body, const vec3 *const restrict r, const vec3 *const restrict p);
#endif

void physRigidBodySetScale(physicsRigidBody *const restrict body, const vec3 scale);

void physRigidBodyCentroidFromPosition(physicsRigidBody *const restrict body);
void physRigidBodyPositionFromCentroid(physicsRigidBody *const restrict body);
void physRigidBodyUpdateGlobalInertia(physicsRigidBody *const restrict body);
void physRigidBodyUpdate(physicsRigidBody *const restrict body, physicsIsland *const restrict island, const float dt);

void physRigidBodyDefDelete(physicsRigidBodyDef *const restrict bodyDef);
void physRigidBodyDelete(physicsRigidBody *const restrict body);


/**
Load colliders.
Load bodies {
	How do we generate the centroid inertia tensor?
	We could do one of the following:
		Store an array of mat3s and centroids (one pair per collider)
		and a single vertex mass array (one used by every collider).

		Store an array of vertex mass arrays. Each array also contains
		the collider's mass and, possibly for hulls, an element at the
		end (containing NaN or something) that specifies whether or not
		the collider's vertices should be weighted.

		Add each collider's contribution to the centroid and inertia
		tensor the moment we finish loading it. This would require an
		extra division per collider and would also need the same vertex
		mass array used by the first method.
}

Component-wise addition for matrices (specifically mat3s)?
Strtoul base? Maybe check other strtox functions, too.
**/


#endif