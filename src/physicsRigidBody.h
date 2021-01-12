#ifndef physicsRigidBody_h
#define physicsRigidBody_h


#include <stdint.h>

#include "vec3.h"
#include "quat.h"
#include "mat3.h"
#include "transform.h"

#include "physicsCollider.h"
#include "physicsConstraintPair.h"

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
#define PHYSRIGIDBODY_COLLISION_MODIFIED 0x08
// The body has been transformed in some way.
#define PHYSRIGIDBODY_TRANSLATED         0x10
#define PHYSRIGIDBODY_ROTATED            0x20
#define PHYSRIGIDBODY_TRANSFORMED        0x30

#ifndef PHYSRIGIDBODY_DEFAULT_STATE
	#define PHYSRIGIDBODY_DEFAULT_STATE (PHYSRIGIDBODY_SIMULATE | PHYSRIGIDBODY_COLLIDE)
#endif

#define physRigidBodySimulateLinear(body) flagsSet(body->flags, PHYSRIGIDBODY_SIMULATE_LINEAR)
#define physRigidBodySimulateAngular(body) flagsSet(body->flags, PHYSRIGIDBODY_SIMULATE_ANGULAR)
#define physRigidBodySimulate(body) flagsSet(body->flags, PHYSRIGIDBODY_SIMULATE)

#define physRigidBodyIgnoreLinear(body) flagsUnset(body->flags, PHYSRIGIDBODY_SIMULATE_LINEAR)
#define physRigidBodyIgnoreAngular(body) flagsUnset(body->flags, PHYSRIGIDBODY_SIMULATE_ANGULAR)
#define physRigidBodyIgnoreSimulation(body) flagsUnset(body->flags, PHYSRIGIDBODY_SIMULATE)

#define physRigidBodyIsSimulated(body) flagsAreSet(body->flags, PHYSRIGIDBODY_SIMULATE)
#define physRigidBodyIsCollidable(body) flagsAreSet(body->flags, PHYSRIGIDBODY_COLLIDE)


typedef struct physicsRigidBodyDef {
	#warning "We don't allow colliders to be added, so why not make this a normal array?"
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

	#warning "We don't allow colliders to be added, so why not make this a normal array?"
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

	// Rigid bodies store doubly linked lists of active joints.
	// These lists are mostly sorted according to the addresses
	// of the second rigid body involved in the joint. Check the
	// explanation given in "physicsConstraintPair.h".
	//
	// Note that the list is actually allocated and managed by the
	// island, this just stores a pointer to the body's first joint.
	physicsJointPair *joints;

	flags_t flags;
} physicsRigidBody;


void physRigidBodyDefInit(physicsRigidBodyDef *const restrict bodyDef);
void physRigidBodyInit(physicsRigidBody *const restrict body, const physicsRigidBodyDef *const restrict bodyDef);

return_t physRigidBodyDefLoad(physicsRigidBodyDef **const restrict bodies, const char *const restrict bodyPath, const size_t bodyPathLength);

void physRigidBodyDefAddCollider(
	physicsRigidBodyDef *const restrict bodyDef, const float mass,
	const vec3 *const restrict centroid, mat3 inertia
);

void physRigidBodySimulateCollisions(physicsRigidBody *const restrict body);
void physRigidBodyIgnoreCollisions(physicsRigidBody *const restrict body);

return_t physRigidBodyPermitCollision(const physicsRigidBody *bodyA, const physicsRigidBody *bodyB);

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
void physRigidBodyApplyAngularImpulsePosition(physicsRigidBody *const restrict body, vec3 J);
void physRigidBodyApplyAngularImpulsePositionInverse(physicsRigidBody *const restrict body, vec3 J);
void physRigidBodyApplyImpulsePosition(physicsRigidBody *const restrict body, const vec3 *const restrict r, const vec3 *const restrict p);
void physRigidBodyApplyImpulsePositionInverse(physicsRigidBody *const restrict body, const vec3 *const restrict r, const vec3 *const restrict p);
#endif

void physRigidBodySetScale(physicsRigidBody *const restrict body, const vec3 scale);

void physRigidBodyCentroidFromPosition(physicsRigidBody *const restrict body);
void physRigidBodyPositionFromCentroid(physicsRigidBody *const restrict body);
void physRigidBodyUpdatePosition(physicsRigidBody *const restrict body);
void physRigidBodyUpdateGlobalInertia(physicsRigidBody *const restrict body);
void physRigidBodyUpdate(physicsRigidBody *const restrict body, const float dt);

void physRigidBodyDefDelete(physicsRigidBodyDef *const restrict bodyDef);
void physRigidBodyDelete(physicsRigidBody *const restrict body);


#endif