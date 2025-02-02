#include "physicsRigidBody.h"


#include <stdio.h>

#include "utilString.h"
#include "utilFile.h"

#include "collider.h"

#include "modulePhysics.h"


#define PHYSRIGIDBODYDEF_PATH_PREFIX        "."FILE_PATH_DELIMITER_STR"resource"FILE_PATH_DELIMITER_STR"physics"FILE_PATH_DELIMITER_STR
#define PHYSRIGIDBODYDEF_PATH_PREFIX_LENGTH (sizeof(PHYSRIGIDBODYDEF_PATH_PREFIX) - 1)


void physRigidBodyDefInit(physicsRigidBodyDef *const restrict bodyDef){
	bodyDef->colliders = NULL;

	bodyDef->mass = 0.f;
	bodyDef->invMass = 0.f;

	vec3InitZero(&bodyDef->centroid);
	mat3InitZero(&bodyDef->inertia);

	bodyDef->flags = PHYSRIGIDBODY_DEFAULT_STATE;
}

void physRigidBodyInit(physicsRigidBody *const restrict body, const physicsRigidBodyDef *const restrict bodyDef){
	physicsCollider *curCollider = NULL;
	const physicsCollider *curColliderDef = bodyDef->colliders;

	body->base = bodyDef;

	body->colliders = NULL;
	// Instantiate the rigid body's colliders.
	while(curColliderDef != NULL){
		curCollider = modulePhysicsColliderInsertAfter(&body->colliders, curCollider);
		if(curCollider == NULL){
			/** MALLOC FAILED **/
		}
		physColliderInstantiate(curCollider, curColliderDef, body);
		curColliderDef = modulePhysicsColliderNext(curColliderDef);
	}

	body->mass = bodyDef->mass;
	body->invMass = bodyDef->invMass;

	mat3InvertOut(bodyDef->inertia,	&body->invInertiaLocal);

	transformInit(&body->state);

	vec3InitZero(&body->linearVelocity);
	vec3InitZero(&body->angularVelocity);
	vec3InitZero(&body->netForce);
	vec3InitZero(&body->netTorque);

	body->joints = /** ALLOCATE NEW JOINT LIST **/NULL;

	body->flags = bodyDef->flags;
}


// Load a rigid body, including any of its colliders.
#warning "Maybe update this like the other loading functions?"
return_t physRigidBodyDefLoad(physicsRigidBodyDef **const restrict bodies, const char *const restrict bodyPath, const size_t bodyPathLength){
	FILE *bodyFile;
	char bodyFullPath[FILE_MAX_PATH_LENGTH];

	// Generate the full path for the rigid body!
	fileGenerateFullResourcePath(
		PHYSRIGIDBODYDEF_PATH_PREFIX, PHYSRIGIDBODYDEF_PATH_PREFIX_LENGTH,
		bodyPath, bodyPathLength,
		bodyFullPath
	);


	// Load the rigid body!
	bodyFile = fopen(bodyFullPath, "r");
	if(bodyFile != NULL){
		return_t success = 0;

		physicsRigidBodyDef bodyDef;
		physicsCollider *curCollider = NULL;
		float curMass = 0.f;
		vec3 curCentroid;
		mat3 curInertia;
		byte_t colliderLoaded = 0;

		char *tokPos;

		char lineBuffer[FILE_MAX_LINE_LENGTH];
		char *line;
		size_t lineLength;


		physRigidBodyDefInit(&bodyDef);


		while((line = fileReadLine(bodyFile, &lineBuffer[0], &lineLength)) != NULL){
			if(curCollider != NULL){
				// New collider.
				if(lineLength >= 2 && line[0] == 'c' && line[lineLength - 1] == '{'){
					// Load the collider using its respective function.
					colliderLoaded = colliderLoad(&curCollider->global, bodyFile, &curCentroid, &curInertia);

				// Physics collider mass.
				}else if(lineLength >= 3 && memcmp(line, "m ", 2) == 0){
					curMass = strtof(&line[2], &tokPos);
					// If the mass specified is invalid, use the default value.
					if(&line[2] == tokPos){
						curMass = PHYSCOLLIDER_DEFAULT_MASS;
					}

				// Physics collider density.
				}else if(lineLength >= 3 && memcmp(line, "d ", 2) == 0){
					curCollider->density = strtof(&line[2], &tokPos);
					// If the density specified is invalid, use the default value.
					if(&line[2] == tokPos){
						curCollider->density = PHYSCOLLIDER_DEFAULT_DENSITY;
					}

				// Physics collider friction.
				}else if(lineLength >= 3 && memcmp(line, "f ", 2) == 0){
					curCollider->friction = strtof(&line[2], &tokPos);
					// If the friction specified is invalid, use the default value.
					if(&line[2] == tokPos){
						curCollider->friction = PHYSCOLLIDER_DEFAULT_FRICTION;
					}

				// Physics collider restitution.
				}else if(lineLength >= 3 && memcmp(line, "r ", 2) == 0){
					curCollider->restitution = strtof(&line[2], &tokPos);
					// If the restitution specified is invalid, use the default value.
					if(&line[2] == tokPos){
						curCollider->restitution = PHYSCOLLIDER_DEFAULT_RESTITUTION;
					}

				// Physics collider end.
				}else if(lineLength >= 1 && line[0] == '}'){
					// Add the new physics collider to our rigid body!
					if(colliderLoaded){
						physRigidBodyDefAddCollider(&bodyDef, curMass, &curCentroid, curInertia);
						success = 1;

					// If there was an error, delete the physics collider.
					// This will also delete the collider we failed to
					// load, and will also free any memory it allocated.
					}else{
						physColliderDeleteBase(curCollider);
					}
					curCollider = NULL;
				}
			}else{
				// Bone name.
				if(lineLength >= 3 && memcmp(line, "b ", 2) == 0){
					const char *boneName;
					// Find the name of the bone to attach this rigid body to!
					const size_t boneNameLength = stringMultiDelimited(&line[2], lineLength - 2, "\" ", &boneName);

					/** What do we do with the bone's name? **/

				// New physics collider.
				}else if(lineLength >= 2 && line[0] == 'p' && line[lineLength - 1] == '{'){
					colliderType currentType = strtoul(&line[2], &tokPos, 10);

					// Make sure a valid number was entered.
					if(tokPos != &line[2]){
						// Check which sort of collider we're loading.
						#warning "This should be 'currentType < COLLIDER_NUM_TYPES'."
						#warning "Please remember to remove the 'currentType=0' statement on the next line."
						if(currentType == 3){currentType=0;
							bodyDef.colliders = modulePhysicsColliderPrepend(&bodyDef.colliders);
							if(bodyDef.colliders == NULL){
								/** MALLOC FAILED **/
							}
							curCollider = bodyDef.colliders;
							curMass = PHYSCOLLIDER_DEFAULT_MASS;
							physColliderInit(curCollider, currentType);

						// If an invalid type was specified, ignore the collider.
						}else{
							printf(
								"Error loading rigid body!\n"
								"Path: %s\n"
								"Line: %s\n"
								"Error: Ignoring invalid collider of type '%i'.\n",
								bodyFullPath, line, currentType
							);
						}
					}else{
						printf(
							"Error loading rigid body!\n"
							"Path: %s\n"
							"Line: %s\n"
							"Error: Ignoring invalid collider of non-numerical type.",
							bodyFullPath, line
						);
					}
				}
			}
		}

		fclose(bodyFile);


		// If we loaded at least one collider, add the
		// rigid body to the beginning of the "bodies" array!
		if(success){
			*bodies = modulePhysicsRigidBodyDefPrepend(bodies);
			if(*bodies == NULL){
				/** MALLOC FAILED **/
			}
			**bodies = bodyDef;
		}

		return(success);
	}else{
		printf(
			"Unable to open rigid body file!\n"
			"Path: %s\n",
			bodyFullPath
		);
	}


	return(0);
}


/*
** Adds a physics collider to a rigid body's list. This
** assumes that the collider's centroid and moment of
** inertia tensor have already been calculated. Note
** that this assumes that the body's inertia tensor has
** note yet been inverted. It will not invert it, either.
*/
void physRigidBodyDefAddCollider(
	physicsRigidBodyDef *const restrict bodyDef, const float mass,
	const vec3 *const restrict centroid, mat3 inertia
){

	vec3 tempCentroid;
	vec3 tempCentroidWeighted;
	vec3 tempCentroidSquaredWeighted;


	// Calculate the collider's contribution to the body's centroid.
	vec3MultiplyS(&bodyDef->centroid, bodyDef->mass);
	vec3FmaP2(mass, centroid, &bodyDef->centroid);

	// Calculate the new mass and inverse mass.
	bodyDef->mass += mass;
	bodyDef->invMass = (bodyDef->mass == 0.f) ? 0.f : 1.f / bodyDef->mass;

	// Scale the centroid by the new inverse mass.
	vec3MultiplyS(&bodyDef->centroid, bodyDef->invMass);


	/*
	** Add this collider's contribution to the inertia tensor.
	** We use the parallel axis theorem to translate it from
	** the collider's centroid to the rigid body's centroid.
	**
	** I = J + m * (dot(R, R) * E - out(R, R))
	**
	** I = new inertia tensor
	** J = old inertia tensor
	** m = mass
	** R = vector displacement from old to new center of mass
	** E = 3x3 identity matrix
	**
	** dot(v, u) = dot product of v and u
	** out(v, u) = outer product of v and u
	**
	**
	** This can be simplified as follows to reduce matrix operations:
	**
	** d = dot(R, R) = xx + yy + zz
	**
	**      [d 0 0]
	** dE = [0 d 0]
	**      [0 0 d]
	**
	**                 [xx yx zx]
	** P = out(R, R) = [xy yy zy]
	**                 [xz yz zz]
	**
	**          [(yy + zz)    -yx       -zx   ]
	**          [   -xy    (xx + zz)    -zy   ]
	** dE - P = [   -xz       -yz    (xx + yy)]
	**
	** I = J + m * (dE - P)
	*/

	vec3SubtractVec3Out(&bodyDef->centroid, centroid, &tempCentroid);
	vec3MultiplySOut(&tempCentroid, mass, &tempCentroidWeighted);
	vec3MultiplyVec3Out(&tempCentroid, &tempCentroidWeighted, &tempCentroidSquaredWeighted);

	// Translate the inertia tensor using the rigid body's centroid.
	// (yy + zz) * m
	inertia.m[0][0] += tempCentroidSquaredWeighted.y + tempCentroidSquaredWeighted.z;
	// xy * m
	inertia.m[0][1] -= tempCentroid.x * tempCentroidWeighted.y;
	// xz * m
	inertia.m[0][2] -= tempCentroid.x * tempCentroidWeighted.z;
	// (xx + zz) * m
	inertia.m[1][1] += tempCentroidSquaredWeighted.x + tempCentroidSquaredWeighted.z;
	// yz * m
	inertia.m[1][2] -= tempCentroid.y * tempCentroidWeighted.z;
	// (xx + yy) * m
	inertia.m[2][2] += tempCentroidSquaredWeighted.x + tempCentroidSquaredWeighted.y;

	// Add the collider's contribution to the body's inertia tensor.
	bodyDef->inertia.m[0][0] += inertia.m[0][0];
	bodyDef->inertia.m[0][1] += inertia.m[0][1];
	bodyDef->inertia.m[0][2] += inertia.m[0][2];
	bodyDef->inertia.m[1][0] = bodyDef->inertia.m[0][1];
	bodyDef->inertia.m[1][1] += inertia.m[1][1];
	bodyDef->inertia.m[1][2] += inertia.m[1][2];
	bodyDef->inertia.m[2][0] = bodyDef->inertia.m[0][2];
	bodyDef->inertia.m[2][1] = bodyDef->inertia.m[1][2];
	bodyDef->inertia.m[2][2] += inertia.m[2][2];
}


void physRigidBodySimulateLinear(physicsRigidBody *const restrict body){
	flagsSet(body->flags, PHYSRIGIDBODY_SIMULATE_LINEAR);
}

void physRigidBodySimulateAngular(physicsRigidBody *const restrict body){
	flagsSet(body->flags, PHYSRIGIDBODY_SIMULATE_ANGULAR);
}

void physRigidBodySimulate(physicsRigidBody *const restrict body){
	flagsSet(body->flags, PHYSRIGIDBODY_SIMULATE);
}

void physRigidBodySimulateCollisions(physicsRigidBody *const restrict body){
	flagsSet(body->flags, PHYSRIGIDBODY_COLLIDE | PHYSRIGIDBODY_COLLISION_MODIFIED);
}


void physRigidBodyIgnoreLinear(physicsRigidBody *const restrict body){
	flagsUnset(body->flags, PHYSRIGIDBODY_SIMULATE_LINEAR);
}

void physRigidBodyIgnoreAngular(physicsRigidBody *const restrict body){
	flagsUnset(body->flags, PHYSRIGIDBODY_SIMULATE_ANGULAR);
}

void physRigidBodyIgnoreSimulation(physicsRigidBody *const restrict body){
	flagsUnset(body->flags, PHYSRIGIDBODY_SIMULATE);
}

void physRigidBodyIgnoreCollisions(physicsRigidBody *const restrict body){
	flagsUnset(body->flags, PHYSRIGIDBODY_COLLIDE);
	flagsSet(body->flags, PHYSRIGIDBODY_COLLISION_MODIFIED);
}


return_t physRigidBodyIsSimulated(physicsRigidBody *const restrict body){
	return(flagsContainsSubset(body->flags, PHYSRIGIDBODY_SIMULATE));
}

return_t physRigidBodyIsCollidable(physicsRigidBody *const restrict body){
	return(flagsContainsSubset(body->flags, PHYSRIGIDBODY_COLLIDE));
}


/*
** Check whether two rigid bodies are allowed to collide.
** The only time we don't allow collision is if both bodies
** are the same or there exists a joint between them that
** does not allow collision.
*/
return_t physRigidBodyPermitCollision(const physicsRigidBody *const bodyA, const physicsRigidBody *const bodyB){
	if(bodyA < bodyB){
		const physicsJoint *curJoint = bodyA->joints;
		while(curJoint != NULL && bodyB >= curJoint->bodyB){
			// We only allow these bodies to collide if
			// every single joint they share allows collision.
			if(bodyB == curJoint->bodyB /*&& curJoint->flags == PHYSJOINT_ALLOW_COLLISION*/){
				//return(0);
			}
			curJoint = curJoint->nextA;
		}

		return(1);
	}else if(bodyA > bodyB){
		const physicsJoint *curJoint = bodyB->joints;
		while(curJoint != NULL && bodyA >= curJoint->bodyB){
			// We only allow these bodies to collide if
			// every single joint they share allows collision.
			if(bodyA == curJoint->bodyB /*&& curJoint->flags == PHYSJOINT_ALLOW_COLLISION*/){
				//return(0);
			}
			curJoint = curJoint->nextA;
		}

		return(1);
	}

	return(0);
}


/*
** Calculate the body's increase in velocity for
** the current timestep using symplectic Euler.
**
** v_{n + 1} = v_n + F * m^{-1} * dt
** w_{n + 1} = w_n + T * I^{-1} * dt
*/
void physRigidBodyIntegrateVelocity(physicsRigidBody *const restrict body, const float dt){
	if(flagsContainsSubset(body->flags, PHYSRIGIDBODY_SIMULATE_LINEAR)){
		// Add the linear acceleration to the linear velocity.
		vec3FmaP2(body->invMass * dt, &body->netForce, &body->linearVelocity);
	}else{
		vec3InitZero(&body->linearVelocity);
	}

	if(flagsContainsSubset(body->flags, PHYSRIGIDBODY_SIMULATE_ANGULAR)){
		vec3 angularAcceleration;

		// Update the global inertia tensor.
		physRigidBodyUpdateGlobalInertia(body);

		// Calculate the body's angular acceleration.
		vec3MultiplySOut(&body->netTorque, dt, &angularAcceleration);
		mat3MultiplyVec3(&body->invInertiaGlobal, &angularAcceleration);
		// Add the angular acceleration to the angular velocity.
		vec3AddVec3(&body->angularVelocity, &angularAcceleration);
	}else{
		mat3InitZero(&body->invInertiaGlobal);
		vec3InitZero(&body->angularVelocity);
	}
}

// Reset the forces acting on a rigid body.
void physRigidBodyResetAccumulators(physicsRigidBody *const restrict body){
	vec3InitZero(&body->netForce);
	vec3InitZero(&body->netTorque);
}

/*
** Calculate the body's change in position for
** the current timestep using symplectic Euler.
**
** x_{n + 1} = x_n + v_{n + 1} * dt
** dq/dt = 0.5 * w * q
** q_{n + 1} = q_n + dq/dt * dt
*/
void physRigidBodyIntegratePosition(physicsRigidBody *const restrict body, const float dt){
	if(flagsContainsSubset(body->flags, PHYSRIGIDBODY_SIMULATE_LINEAR)){
		// Compute the object's new position.
		vec3FmaP2(dt, &body->linearVelocity, &body->centroid);
		flagsSet(body->flags, PHYSRIGIDBODY_TRANSLATED);
	}else{
		flagsUnset(body->flags, PHYSRIGIDBODY_TRANSLATED);
	}

	if(flagsContainsSubset(body->flags, PHYSRIGIDBODY_SIMULATE_ANGULAR)){
		// Calculate the change in orientation.
		quatIntegrate(&body->state.rot, &body->angularVelocity, dt);
		// Don't forget to normalize it, as
		// this process can introduce errors.
		quatNormalizeQuatFast(&body->state.rot);

		flagsSet(body->flags, PHYSRIGIDBODY_ROTATED);
	}else{
		flagsUnset(body->flags, PHYSRIGIDBODY_ROTATED);
	}
}


// Add a translational force to a rigid body.
void physRigidBodyApplyLinearForce(physicsRigidBody *const restrict body, const vec3 *const restrict F){
	vec3AddVec3(&body->netForce, F);
}

// Add a rotational force to a rigid body.
void physRigidBodyApplyAngularForce(physicsRigidBody *const restrict body, const vec3 *const restrict r, const vec3 *const restrict F){
	vec3 torque;
	vec3SubtractVec3Out(r, &body->centroid, &torque);
	vec3CrossVec3P1(&torque, F);
	vec3AddVec3(&body->netTorque, &torque);
}

// Add a translational and rotational force to a rigid body.
void physRigidBodyApplyForce(physicsRigidBody *const restrict body, const vec3 *const restrict r, const vec3 *const restrict F){
	physRigidBodyApplyLinearForce(body, F);
	physRigidBodyApplyAngularForce(body, r, F);

}


// Add a translational impulse to a rigid body.
void physRigidBodyApplyLinearImpulse(physicsRigidBody *const restrict body, const vec3 *const restrict J){
	// Linear velocity.
	vec3FmaP2(body->invMass, J, &body->linearVelocity);
}

// Subtract a translational impulse from a rigid body.
void physRigidBodyApplyLinearImpulseInverse(physicsRigidBody *const restrict body, const vec3 *const restrict J){
	// Linear velocity.
	vec3FmaP2(-body->invMass, J, &body->linearVelocity);
}

// Add a rotational impulse to a rigid body.
void physRigidBodyApplyAngularImpulse(physicsRigidBody *const restrict body, vec3 J){
	// Angular velocity.
	mat3MultiplyVec3(&body->invInertiaGlobal, &J);
	vec3AddVec3(&body->angularVelocity, &J);
}

// Subtract a rotational impulse from a rigid body.
void physRigidBodyApplyAngularImpulseInverse(physicsRigidBody *const restrict body, vec3 J){
	// Angular velocity.
	mat3MultiplyVec3(&body->invInertiaGlobal, &J);
	vec3SubtractVec3P1(&body->angularVelocity, &J);
}

// Add a translational and rotational impulse to a rigid body.
void physRigidBodyApplyImpulse(physicsRigidBody *const restrict body, const vec3 *const restrict r, const vec3 *const restrict J){
	vec3 impulse;

	// Linear velocity.
	vec3FmaP2(body->invMass, J, &body->linearVelocity);

	// Angular velocity.
	vec3CrossVec3Out(r, J, &impulse);
	mat3MultiplyVec3(&body->invInertiaGlobal, &impulse);
	vec3AddVec3(&body->angularVelocity, &impulse);
}

// Subtract a translational and rotational impulse from a rigid body.
void physRigidBodyApplyImpulseInverse(physicsRigidBody *const restrict body, const vec3 *const restrict r, const vec3 *const restrict J){
	vec3 impulse;

	// Linear velocity.
	vec3FmaP2(-body->invMass, J, &body->linearVelocity);

	// Angular velocity.
	vec3CrossVec3Out(r, J, &impulse);
	mat3MultiplyVec3(&body->invInertiaGlobal, &impulse);
	vec3SubtractVec3P1(&body->angularVelocity, &impulse);
}

// Add a translational and "boosted" rotational impulse to a rigid body.
void physRigidBodyApplyImpulseBoost(
	physicsRigidBody *const restrict body, const vec3 *const restrict r,
	const vec3 *const restrict J, const vec3 *const restrict a
){

	vec3 impulse;

	// Linear velocity.
	vec3FmaP2(body->invMass, J, &body->linearVelocity);

	// Angular velocity.
	vec3CrossVec3Out(r, J, &impulse);
	vec3AddVec3(&impulse, a);
	mat3MultiplyVec3(&body->invInertiaGlobal, &impulse);
	vec3AddVec3(&body->angularVelocity, &impulse);
}

// Subtract a translational and "boosted" rotational impulse from a rigid body.
void physRigidBodyApplyImpulseBoostInverse(
	physicsRigidBody *const restrict body, const vec3 *const restrict r,
	const vec3 *const restrict J, const vec3 *const restrict a
){

	vec3 impulse;

	// Linear velocity.
	vec3FmaP2(-body->invMass, J, &body->linearVelocity);

	// Angular velocity.
	vec3CrossVec3Out(r, J, &impulse);
	vec3AddVec3(&impulse, a);
	mat3MultiplyVec3(&body->invInertiaGlobal, &impulse);
	vec3SubtractVec3P1(&body->angularVelocity, &impulse);
}

#ifdef PHYSCOLLIDER_USE_POSITIONAL_CORRECTION
// Add a rotational impulse to a rigid body's position.
void physRigidBodyApplyLinearImpulsePosition(physicsRigidBody *const restrict body, const vec3 *const restrict J){
	if(flagsContainsSubset(body->flags, PHYSRIGIDBODY_SIMULATE_LINEAR)){
		vec3FmaP2(body->invMass, J, &body->centroid);
	}
}

// Subtract a rotational impulse from a rigid body's position.
void physRigidBodyApplyLinearImpulsePositionInverse(physicsRigidBody *const restrict body, const vec3 *const restrict J){
	if(flagsContainsSubset(body->flags, PHYSRIGIDBODY_SIMULATE_LINEAR)){
		vec3FmaP2(-body->invMass, J, &body->centroid);
	}
}

// Add a rotational impulse to a rigid body's position.
void physRigidBodyApplyAngularImpulsePosition(physicsRigidBody *const restrict body, vec3 J){
	if(flagsContainsSubset(body->flags, PHYSRIGIDBODY_SIMULATE_ANGULAR)){
		quat tempRot;
		mat3MultiplyVec3(&body->invInertiaGlobal, &J);
		quatDifferentiateOut(&body->state.rot, &J, &tempRot);
		quatAddQuat(&body->state.rot, &tempRot);
		quatNormalizeQuatFast(&body->state.rot);

		physRigidBodyUpdateGlobalInertia(body);
	}
}

// Subtract a rotational impulse from a rigid body's position.
void physRigidBodyApplyAngularImpulsePositionInverse(physicsRigidBody *const restrict body, vec3 J){
	if(flagsContainsSubset(body->flags, PHYSRIGIDBODY_SIMULATE_ANGULAR)){
		quat tempRot;
		mat3MultiplyVec3(&body->invInertiaGlobal, &J);
		quatDifferentiateOut(&body->state.rot, &J, &tempRot);
		quatSubtractQuatP1(&body->state.rot, &tempRot);
		quatNormalizeQuatFast(&body->state.rot);

		physRigidBodyUpdateGlobalInertia(body);
	}
}

// Add a translational and rotational impulse to a rigid body's position.
void physRigidBodyApplyImpulsePosition(physicsRigidBody *const restrict body, const vec3 *const restrict r, const vec3 *const restrict J){
	// Position.
	if(flagsContainsSubset(body->flags, PHYSRIGIDBODY_SIMULATE_LINEAR)){
		vec3FmaP2(body->invMass, J, &body->centroid);
	}

	// Orientation.
	if(flagsContainsSubset(body->flags, PHYSRIGIDBODY_SIMULATE_ANGULAR)){
		vec3 impulse;
		quat tempRot;
		vec3CrossVec3Out(r, J, &impulse);
		mat3MultiplyVec3(&body->invInertiaGlobal, &impulse);
		quatDifferentiateOut(&body->state.rot, &impulse, &tempRot);
		quatAddQuat(&body->state.rot, &tempRot);
		quatNormalizeQuatFast(&body->state.rot);

		physRigidBodyUpdateGlobalInertia(body);
	}
}

// Subtract a translational and rotational impulse from a rigid body's position.
void physRigidBodyApplyImpulsePositionInverse(physicsRigidBody *const restrict body, const vec3 *const restrict r, const vec3 *const restrict J){
	// Position.
	if(flagsContainsSubset(body->flags, PHYSRIGIDBODY_SIMULATE_LINEAR)){
		vec3FmaP2(-body->invMass, J, &body->centroid);
	}

	// Orientation.
	if(flagsContainsSubset(body->flags, PHYSRIGIDBODY_SIMULATE_ANGULAR)){
		vec3 impulse;
		quat tempRot;
		vec3CrossVec3Out(r, J, &impulse);
		mat3MultiplyVec3(&body->invInertiaGlobal, &impulse);
		quatDifferentiateOut(&body->state.rot, &impulse, &tempRot);
		quatSubtractQuatP1(&body->state.rot, &tempRot);
		quatNormalizeQuatFast(&body->state.rot);

		physRigidBodyUpdateGlobalInertia(body);
	}
}
#endif


void physRigidBodySetScale(physicsRigidBody *const restrict body, const vec3 scale){
	mat3 inertia = body->base->inertia;

	const float xy = scale.x * scale.y;
	const float xz = scale.x * scale.z;
	const float yz = scale.y * scale.z;

	// Note: Scaling the inverse inertia tensor may seem
	// difficult at first, but there is a solution. When
	// we're calculating a body's local inertia tensor,
	// we only need six unique numbers. These are:
	//
	// I[0] += yy + zz,
	// I[1] += xx + zz,
	// I[2] += xx + yy,
	// I[3] -= x * y,
	// I[4] -= x * z,
	// I[5] -= y * z.
	//
	// Scaling I[3], I[4] and I[5] is trivial - we simply
	// multiply each component by the product of the axes
	// scales. The other three, however, present a problem.
	//
	//
	// Luckily, it's not impossible. We can actually
	// calculate the values of xx, yy and zz summed for
	// every vertex. Note that once we calculate one,
	// we can calculate the others using a subtraction:
	//
	// xx = 0.5f * (-I[0] + I[1] + I[2]),
	//    = I[1] - zz,
	//    = I[2] - yy,
	// yy = 0.5f * ( I[0] - I[1] + I[2]),
	//    = I[0] - zz,
	//    = I[2] - xx,
	// zz = 0.5f * ( I[0] + I[1] - I[2]),
	//    = I[0] - yy,
	//    = I[1] - xx.
	//
	// With these values, we can easily scale I[0], I[1]
	// and I[2]. We need to multiply by the square of the
	// scale, as these values represent squares.
	//
	// I[0] = yy * sy^2 + zz * sz^2,
	// I[1] = xx * sx^2 + zz * sz^2,
	// I[2] = xx * sx^2 + yy * sy^2.
	//
	//
	// The only problem is that we can only apply this
	// to non-inverted inertia tensors.

	float sqrZ = (inertia.m[0][0] + inertia.m[1][1] - inertia.m[2][2]) * 0.5f;
	const float sqrY = (inertia.m[0][0] - sqrZ) * scale.y * scale.y;
	const float sqrX = (inertia.m[1][1] - sqrZ) * scale.x * scale.x;
	sqrZ *= scale.z * scale.z;

	inertia.m[0][0] = sqrY + sqrZ;
	inertia.m[0][1] *= xy;
	inertia.m[0][2] *= xz;
	inertia.m[1][0] *= xy;
	inertia.m[1][1] = sqrX + sqrZ;
	inertia.m[1][2] *= yz;
	inertia.m[2][0] *= xz;
	inertia.m[2][1] *= yz;
	inertia.m[2][2] = sqrX + sqrY;

	mat3InvertOut(inertia, &body->invInertiaLocal);
	#ifdef TRANSFORM_MATRIX_SHEAR
	body->state.scale.m[0][0] = scale.x;
	body->state.scale.m[1][1] = scale.y;
	body->state.scale.m[2][2] = scale.z;
	#else
	body->state.scale = scale;
	#endif
}


// Compute the rigid body's centroid from its transform position.
void physRigidBodyCentroidFromPosition(physicsRigidBody *const restrict body){
	transformPointOut(&body->state, &body->base->centroid, &body->centroid);
}

// Compute the rigid body's transform position from its centroid.
void physRigidBodyPositionFromCentroid(physicsRigidBody *const restrict body){
	vec3NegateOut(&body->base->centroid, &body->state.pos);
	transformDirection(&body->state, &body->state.pos);
	vec3AddVec3(&body->state.pos, &body->centroid);
}

void physRigidBodyUpdatePosition(physicsRigidBody *const restrict body){
	if(flagsContainsSubset(body->flags, PHYSRIGIDBODY_TRANSFORMED)){
		physRigidBodyPositionFromCentroid(body);
	}
}

void physRigidBodyUpdateGlobalInertia(physicsRigidBody *const restrict body){
	mat3 orientation;
	mat3 invOrientation;
	// Denote the body's orientation matrix R and it's inverse R^{-1}.
	quatToMat3(&body->state.rot, &orientation);
	mat3TransposeOut(orientation, &invOrientation);

	// The global inverse inertia tensor is given by
	//
	// I_global^{-1} = R * I_local^{-1} * R^{-1}.
	//
	// When applying impulses, we rely on the following relation:
	//
	// R * I_local^{-1} * R^{-1} * L_global = R * I_local^{-1} * L_local
	// = R * w_local
	// = w_global.
	//
	// The orientation matrix is used to transform the angular momentum
	// to local space, then the angular velocity to global space.
	mat3MultiplyMat3Out(orientation, body->invInertiaLocal, &body->invInertiaGlobal);
	mat3MultiplyMat3P1(&body->invInertiaGlobal, invOrientation);
}

/*
** Update a rigid body. This involves moving and
** rotating its centroid and inertia tensor as
** well as integrating its velocity using any forces.
*/
void physRigidBodyUpdate(physicsRigidBody *const restrict body, const float dt){
	// Apply gravity.
	const vec3 gravity = {.x = 0.f, .y = PHYSRIGIDBODY_GRAVITY * body->mass, .z = 0.f};
	physRigidBodyApplyLinearForce(body, &gravity);
	// Update the body's velocity.
	physRigidBodyIntegrateVelocity(body, dt);
	physRigidBodyResetAccumulators(body);

	physRigidBodyPositionFromCentroid(body);
}


void physRigidBodyDefDelete(physicsRigidBodyDef *const restrict bodyDef){
	modulePhysicsColliderBaseFreeArray(&bodyDef->colliders);
}

void physRigidBodyDelete(physicsRigidBody *const restrict body){
	modulePhysicsColliderInstanceFreeArray(&body->colliders);
}