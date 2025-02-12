#ifndef physicsRigidBody_h
#define physicsRigidBody_h


#include <stdint.h>

#include "settingsPhysics.h"

#include "vec3.h"
#include "quat.h"
#include "mat3.h"
#include "transform.h"

#include "physicsCollider.h"
#include "physicsJoint.h"

#include "utilTypes.h"


#warning "If these flags aren't set but the rigid body's mass is non-zero, we get funky results!"
#warning "The best way to fix this would be to have functions to get the mass or inertia tensor, which return zero unless the flags are set."
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

#define PHYSRIGIDBODY_DEFAULT_STATE (PHYSRIGIDBODY_SIMULATE | PHYSRIGIDBODY_COLLIDE | PHYSRIGIDBODY_TRANSFORMED)

// Use the default gravity value if none was defined.
#ifndef PHYSRIGIDBODY_GRAVITY
	#define PHYSRIGIDBODY_GRAVITY -9.80665f
#endif


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
	flags8_t flags;
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

	// Current transformation of the body's origin.
	// The position stored here should not be
	// confused with the rigid body's centroid!
	transform state;

	// These roperties control the body's motion.
	vec3 linearVelocity;
	vec3 angularVelocity;
	vec3 netForce;
	vec3 netTorque;

	// Rigid bodies store doubly linked lists of active joints.
	// All of the joints "owned" by this body (for which this
	// body is body A) are stored at the beginning of the list.
	physicsJoint *joints;

	flags8_t flags;
} physicsRigidBody;


void physRigidBodyDefInit(physicsRigidBodyDef *const restrict bodyDef);
void physRigidBodyInit(physicsRigidBody *const restrict body, const physicsRigidBodyDef *const restrict bodyDef);

return_t physRigidBodyDefLoad(physicsRigidBodyDef **const restrict bodies, const char *const restrict bodyPath, const size_t bodyPathLength);

void physRigidBodyDefAddCollider(
	physicsRigidBodyDef *const restrict bodyDef, const float mass,
	const vec3 *const restrict centroid, mat3 inertia
);

void physRigidBodySimulateLinear(physicsRigidBody *const restrict body);
void physRigidBodySimulateAngular(physicsRigidBody *const restrict body);
void physRigidBodySimulate(physicsRigidBody *const restrict body);
void physRigidBodySimulateCollisions(physicsRigidBody *const restrict body);

void physRigidBodyIgnoreLinear(physicsRigidBody *const restrict body);
void physRigidBodyIgnoreAngular(physicsRigidBody *const restrict body);
void physRigidBodyIgnoreSimulation(physicsRigidBody *const restrict body);
void physRigidBodyIgnoreCollisions(physicsRigidBody *const restrict body);

return_t physRigidBodyIsSimulated(physicsRigidBody *const restrict body);
return_t physRigidBodyIsCollidable(physicsRigidBody *const restrict body);

return_t physRigidBodyPermitCollision(const physicsRigidBody *const bodyA, const physicsRigidBody *const bodyB);

void physRigidBodyIntegrateVelocity(physicsRigidBody *const restrict body, const float dt);
void physRigidBodyResetAccumulators(physicsRigidBody *const restrict body);
void physRigidBodyIntegratePosition(physicsRigidBody *const restrict body, const float dt);

void physRigidBodyApplyLinearForce(physicsRigidBody *const restrict body, const vec3 *const restrict F);
void physRigidBodyApplyAngularForce(physicsRigidBody *const restrict body, const vec3 *const restrict r, const vec3 *const restrict F);
void physRigidBodyApplyForce(physicsRigidBody *const restrict body, const vec3 *const restrict r, const vec3 *const restrict F);

void physRigidBodyApplyLinearImpulse(physicsRigidBody *const restrict body, const vec3 *const restrict J);
void physRigidBodyApplyLinearImpulseInverse(physicsRigidBody *const restrict body, const vec3 *const restrict J);
void physRigidBodyApplyAngularImpulse(physicsRigidBody *const restrict body, vec3 J);
void physRigidBodyApplyAngularImpulseInverse(physicsRigidBody *const restrict body, vec3 J);
void physRigidBodyApplyImpulse(physicsRigidBody *const restrict body, const vec3 *const restrict r, const vec3 *const restrict J);
void physRigidBodyApplyImpulseInverse(physicsRigidBody *const restrict body, const vec3 *const restrict r, const vec3 *const restrict J);
void physRigidBodyApplyImpulseBoost(
	physicsRigidBody *const restrict body, const vec3 *const restrict r,
	const vec3 *const restrict J, const vec3 *const restrict a
);
void physRigidBodyApplyImpulseBoostInverse(
	physicsRigidBody *const restrict body, const vec3 *const restrict r,
	const vec3 *const restrict J, const vec3 *const restrict a
);
#ifdef PHYSCOLLIDER_USE_POSITIONAL_CORRECTION
void physRigidBodyApplyLinearImpulsePosition(physicsRigidBody *const restrict body, const vec3 *const restrict J);
void physRigidBodyApplyLinearImpulsePositionInverse(physicsRigidBody *const restrict body, const vec3 *const restrict J);
void physRigidBodyApplyAngularImpulsePosition(physicsRigidBody *const restrict body, vec3 J);
void physRigidBodyApplyAngularImpulsePositionInverse(physicsRigidBody *const restrict body, vec3 J);
void physRigidBodyApplyImpulsePosition(physicsRigidBody *const restrict body, const vec3 *const restrict r, const vec3 *const restrict J);
void physRigidBodyApplyImpulsePositionInverse(physicsRigidBody *const restrict body, const vec3 *const restrict r, const vec3 *const restrict J);
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