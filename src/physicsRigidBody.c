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
	const physicsCollider *curCollider = bodyDef->colliders;

	body->base = bodyDef;

	// Instantiate the rigid body's colliders.
	while(curCollider != NULL){
		body->colliders = modulePhysicsColliderPrepend(&body->colliders);
		if(body->colliders == NULL){
			/** MALLOC FAILED **/
		}
		physColliderInstantiate(body->colliders, curCollider, body);
		curCollider = modulePhysicsColliderNext(curCollider);
	}

	body->mass = bodyDef->mass;
	body->invMass = bodyDef->invMass;

	mat3InvertOut(bodyDef->inertia,	&body->invInertiaLocal);

	transformStateInit(&body->state);

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
		float curMass;
		vec3 curCentroid;
		mat3 curInertia;
		byte_t colliderLoaded;

		char *tokPos;

		char lineBuffer[FILE_MAX_LINE_LENGTH];
		char *line;
		size_t lineLength;


		physRigidBodyDefInit(&bodyDef);


		while((line = fileReadLine(bodyFile, &lineBuffer[0], &lineLength)) != NULL){
			if(curCollider != NULL){
				// New collider.
				if(memcmp(line, "c ", 2) == 0 && line[lineLength - 1] == '{'){
					// Load the collider using its respective function.
					colliderLoaded = colliderLoad(&curCollider->global, bodyFile, &curCentroid, &curInertia);

				// Physics collider mass.
				}else if(memcmp(line, "m ", 2) == 0){
					curMass = strtof(&line[2], &tokPos);
					// If the mass specified is invalid, use the default value.
					if(&line[2] == tokPos){
						curMass = PHYSCOLLIDER_DEFAULT_MASS;
					}

				// Physics collider density.
				}else if(memcmp(line, "d ", 2) == 0){
					curCollider->density = strtof(&line[2], &tokPos);
					// If the density specified is invalid, use the default value.
					if(&line[2] == tokPos){
						curCollider->density = PHYSCOLLIDER_DEFAULT_DENSITY;
					}

				// Physics collider friction.
				}else if(memcmp(line, "f ", 2) == 0){
					curCollider->friction = strtof(&line[2], &tokPos);
					// If the friction specified is invalid, use the default value.
					if(&line[2] == tokPos){
						curCollider->friction = PHYSCOLLIDER_DEFAULT_FRICTION;
					}

				// Physics collider restitution.
				}else if(memcmp(line, "r ", 2) == 0){
					curCollider->restitution = strtof(&line[2], &tokPos);
					// If the restitution specified is invalid, use the default value.
					if(&line[2] == tokPos){
						curCollider->restitution = PHYSCOLLIDER_DEFAULT_RESTITUTION;
					}

				// Physics collider end.
				}else if(line[0] == '}'){
					// Add the new physics collider to our rigid body!
					if(colliderLoaded){
						physRigidBodyDefAddCollider(&bodyDef, curMass, &curCentroid, curInertia);
						success = 1;

					// If there was an error, delete the physics collider.
					// This will also delete the collider we failed to
					// load, and will also free any memory it allocated.
					}else{
						physColliderDelete(curCollider);
					}
					curCollider = NULL;
				}
			}else{
				// Bone name.
				if(memcmp(line, "b ", 2) == 0){
					size_t boneNameLength;
					// Find the name of the bone to attach this rigid body to!
					const char *const boneName = stringMultiDelimited(&line[2], lineLength - 2, "\" ", &boneNameLength);

					/** What do we do with the bone's name? **/

				// New physics collider.
				}else if(memcmp(line, "p ", 2) == 0 && line[lineLength - 1] == '{'){
					colliderType_t currentType = strtoul(&line[2], &tokPos, 10);

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
			*bodies = modulePhysicsBodyDefPrepend(bodies);
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
	vec3MultiplySOut(centroid, mass, &tempCentroid);
	vec3MultiplyS(&bodyDef->centroid, bodyDef->mass);

	// Calculate the new mass and inverse mass.
	bodyDef->mass += mass;
	bodyDef->invMass = (bodyDef->mass == 0.f) ? 0.f : 1.f / bodyDef->mass;

	// Add this collider's contribution to the centroid.
	vec3AddVec3(&bodyDef->centroid, &tempCentroid);
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

	vec3SubtractVec3FromOut(&bodyDef->centroid, centroid, &tempCentroid);
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


void physRigidBodySimulateCollisions(physicsRigidBody *const restrict body){
	if(!physRigidBodyIsSimulated(body)){
		flagsSet(body->flags, PHYSRIGIDBODY_COLLIDE | PHYSRIGIDBODY_COLLISION_MODIFIED);
	}
}

void physRigidBodyIgnoreCollisions(physicsRigidBody *const restrict body){
	if(physRigidBodyIsSimulated(body)){
		flagsUnset(body->flags, PHYSRIGIDBODY_COLLIDE);
		flagsSet(body->flags, PHYSRIGIDBODY_COLLISION_MODIFIED);
	}
}


/*
** Check whether two rigid bodies are allowed to collide.
** The only time we don't allow collision is if both bodies
** are the same or there exists a joint between them that
** does not allow collision.
*/
return_t physRigidBodyPermitCollision(const physicsRigidBody *bodyA, const physicsRigidBody *bodyB){
	if(bodyA < bodyB){
		const physicsJointPair *curJoint = bodyA->joints;
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
		const physicsJointPair *curJoint = bodyB->joints;
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
** v_(n + 1) = v_n + F * m^(-1) * dt
** w_(n + 1) = w_n + T * I^(-1) * dt
*/
void physRigidBodyIntegrateVelocity(physicsRigidBody *const restrict body, const float dt){
	if(flagsAreSet(body->flags, PHYSRIGIDBODY_SIMULATE_LINEAR)){
		vec3 linearAcceleration;
		// Calculate the body's linear acceleration.
		vec3MultiplySOut(&body->netForce, body->invMass * dt, &linearAcceleration);
		// Add the linear acceleration to the linear velocity.
		vec3AddVec3(&body->linearVelocity, &linearAcceleration);
	}else{
		vec3InitZero(&body->linearVelocity);
	}

	if(flagsAreSet(body->flags, PHYSRIGIDBODY_SIMULATE_ANGULAR)){
		vec3 angularAcceleration;

		// Update the global inertia tensor.
		physRigidBodyUpdateGlobalInertia(body);

		// Calculate the body's angular acceleration.
		vec3MultiplySOut(&body->netTorque, dt, &angularAcceleration);
		mat3MultiplyByVec3(&body->invInertiaGlobal, &angularAcceleration);
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
** x_(n + 1) = x_n + v_(n + 1) * dt
** dq/dt = 0.5 * w * q
** q_(n + 1) = q_n + dq/dt * dt
*/
void physRigidBodyIntegratePosition(physicsRigidBody *const restrict body, const float dt){
	if(flagsAreSet(body->flags, PHYSRIGIDBODY_SIMULATE_LINEAR)){
		vec3 linearVelocityDelta;
		vec3MultiplySOut(&body->linearVelocity, dt, &linearVelocityDelta);
		// Compute the object's new position.
		vec3AddVec3(&body->centroid, &linearVelocityDelta);

		flagsSet(body->flags, PHYSRIGIDBODY_TRANSLATED);
	}else{
		flagsUnset(body->flags, PHYSRIGIDBODY_TRANSLATED);
	}

	if(flagsAreSet(body->flags, PHYSRIGIDBODY_SIMULATE_ANGULAR)){
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
	vec3SubtractVec3FromOut(r, &body->centroid, &torque);
	vec3CrossByVec3(&torque, F);
	vec3AddVec3(&body->netTorque, &torque);
}

// Add a translational and rotational force to a rigid body.
void physRigidBodyApplyForce(physicsRigidBody *const restrict body, const vec3 *const restrict r, const vec3 *const restrict F){
	physRigidBodyApplyLinearForce(body, F);
	physRigidBodyApplyAngularForce(body, r, F);

}


// Add a translational impulse to a rigid body.
void physRigidBodyApplyLinearImpulse(physicsRigidBody *const restrict body, vec3 J){
	// Linear velocity.
	vec3MultiplyS(&J, body->invMass);
	vec3AddVec3(&body->linearVelocity, &J);
}

// Subtract a translational impulse from a rigid body.
void physRigidBodyApplyLinearImpulseInverse(physicsRigidBody *const restrict body, vec3 J){
	// Linear velocity.
	vec3MultiplyS(&J, body->invMass);
	vec3SubtractVec3From(&body->linearVelocity, &J);
}

// Add a rotational impulse to a rigid body.
void physRigidBodyApplyAngularImpulse(physicsRigidBody *const restrict body, vec3 J){
	// Angular velocity.
	mat3MultiplyByVec3(&body->invInertiaGlobal, &J);
	vec3AddVec3(&body->angularVelocity, &J);
}

// Subtract a rotational impulse from a rigid body.
void physRigidBodyApplyAngularImpulseInverse(physicsRigidBody *const restrict body, vec3 J){
	// Angular velocity.
	mat3MultiplyByVec3(&body->invInertiaGlobal, &J);
	vec3SubtractVec3From(&body->angularVelocity, &J);
}

// Add a translational and rotational impulse to a rigid body.
void physRigidBodyApplyImpulse(physicsRigidBody *const restrict body, const vec3 *const restrict r, const vec3 *const restrict p){
	vec3 impulse;

	// Linear velocity.
	vec3MultiplySOut(p, body->invMass, &impulse);
	vec3AddVec3(&body->linearVelocity, &impulse);

	// Angular velocity.
	vec3CrossVec3Out(r, p, &impulse);
	mat3MultiplyByVec3(&body->invInertiaGlobal, &impulse);
	vec3AddVec3(&body->angularVelocity, &impulse);
}

// Add a translational and "boosted" rotational impulse to a rigid body.
void physRigidBodyApplyImpulseBoost(physicsRigidBody *const restrict body, const vec3 *const restrict r, const vec3 *const restrict p, const vec3 *const restrict a){
	vec3 impulse;

	// Linear velocity.
	vec3MultiplySOut(p, body->invMass, &impulse);
	vec3AddVec3(&body->linearVelocity, &impulse);

	// Angular velocity.
	vec3CrossVec3Out(r, p, &impulse);
	vec3AddVec3(&impulse, a);
	mat3MultiplyByVec3(&body->invInertiaGlobal, &impulse);
	vec3AddVec3(&body->angularVelocity, &impulse);
}

// Subtract a translational and rotational impulse from a rigid body.
void physRigidBodyApplyImpulseInverse(physicsRigidBody *const restrict body, const vec3 *const restrict r, const vec3 *const restrict p){
	vec3 impulse;

	// Linear velocity.
	vec3MultiplySOut(p, body->invMass, &impulse);
	vec3SubtractVec3From(&body->linearVelocity, &impulse);

	// Angular velocity.
	vec3CrossVec3Out(r, p, &impulse);
	mat3MultiplyByVec3(&body->invInertiaGlobal, &impulse);
	vec3SubtractVec3From(&body->angularVelocity, &impulse);
}

// Subtract a translational and "boosted" rotational impulse from a rigid body.
void physRigidBodyApplyImpulseBoostInverse(physicsRigidBody *const restrict body, const vec3 *const restrict r, const vec3 *const restrict p, const vec3 *const restrict a){
	vec3 impulse;

	// Linear velocity.
	vec3MultiplySOut(p, body->invMass, &impulse);
	vec3SubtractVec3From(&body->linearVelocity, &impulse);

	// Angular velocity.
	vec3CrossVec3Out(r, p, &impulse);
	vec3AddVec3(&impulse, a);
	mat3MultiplyByVec3(&body->invInertiaGlobal, &impulse);
	vec3SubtractVec3From(&body->angularVelocity, &impulse);
}

#ifdef PHYSCOLLIDER_USE_POSITIONAL_CORRECTION
// Add a translational and rotational impulse to a rigid body's position.
void physRigidBodyApplyImpulsePosition(physicsRigidBody *const restrict body, const vec3 *const restrict r, const vec3 *const restrict p){
	// Position.
	if(flagsAreSet(body->flags, PHYSRIGIDBODY_SIMULATE_LINEAR)){
		vec3 impulse;
		vec3MultiplySOut(p, body->invMass, &impulse);
		vec3AddVec3(&body->centroid, &impulse);
	}

	// Orientation.
	if(flagsAreSet(body->flags, PHYSRIGIDBODY_SIMULATE_ANGULAR)){
		vec3 impulse;
		quat tempRot;
		vec3CrossVec3Out(r, p, &impulse);
		mat3MultiplyByVec3(&body->invInertiaGlobal, &impulse);
		quatDifferentiateOut(&body->state.rot, &impulse, &tempRot);
		quatAddQuat(&body->state.rot, &tempRot);
		quatNormalizeQuatFast(&body->state.rot);

		physRigidBodyUpdateGlobalInertia(body);
	}
}

// Subtract a translational and rotational impulse from a rigid body's position.
void physRigidBodyApplyImpulsePositionInverse(physicsRigidBody *const restrict body, const vec3 *const restrict r, const vec3 *const restrict p){
	// Position.
	if(flagsAreSet(body->flags, PHYSRIGIDBODY_SIMULATE_LINEAR)){
		vec3 impulse;
		vec3MultiplySOut(p, body->invMass, &impulse);
		vec3SubtractVec3From(&body->centroid, &impulse);
	}

	// Orientation.
	if(flagsAreSet(body->flags, PHYSRIGIDBODY_SIMULATE_ANGULAR)){
		vec3 impulse;
		quat tempRot;
		vec3CrossVec3Out(r, p, &impulse);
		mat3MultiplyByVec3(&body->invInertiaGlobal, &impulse);
		quatDifferentiateOut(&body->state.rot, &impulse, &tempRot);
		quatSubtractQuatFrom(&body->state.rot, &tempRot);
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
	body->state.scale = scale;
}


void physRigidBodyCentroidFromPosition(physicsRigidBody *const restrict body){
	vec3MultiplyVec3Out(&body->base->centroid, &body->state.scale, &body->centroid);
	quatRotateVec3Fast(&body->state.rot, &body->centroid);
	vec3AddVec3(&body->centroid, &body->state.pos);
}

void physRigidBodyPositionFromCentroid(physicsRigidBody *const restrict body){
	vec3NegateOut(&body->base->centroid, &body->state.pos);
	vec3MultiplyVec3(&body->state.pos, &body->state.scale);
	quatRotateVec3Fast(&body->state.rot, &body->state.pos);
	vec3AddVec3(&body->state.pos, &body->centroid);
}

void physRigidBodyUpdatePosition(physicsRigidBody *const restrict body){
	if(flagsAreSet(body->flags, PHYSRIGIDBODY_TRANSFORMED)){
		physRigidBodyPositionFromCentroid(body);
	}
}

void physRigidBodyUpdateGlobalInertia(physicsRigidBody *const restrict body){
	mat3 orientation;
	mat3 invOrientation;
	// Denote the body's orientation matrix R and it's inverse R^(-1).
	quatToMat3(&body->state.rot, &orientation);
	mat3TransposeOut(orientation, &invOrientation);

	// The global inverse inertia tensor is given by
	//
	// I_global^(-1) = R * I_local^(-1) * R^(-1).
	//
	// When applying impulses, we rely on the following relation:
	//
	// R * I_local^(-1) * R^(-1) * L_global = R * I_local^(-1) * L_local
	// = R * w_local
	// = w_global.
	//
	// The orientation matrix is used to transform the angular momentum
	// to local space, then the angular velocity to global space.
	mat3MultiplyByMat3Out(body->invInertiaLocal, orientation, &body->invInertiaGlobal);
	mat3MultiplyMat3By(&body->invInertiaGlobal, invOrientation);
}

/*
** Update a rigid body. This involves moving and
** rotating its centroid and inertia tensor as
** well as integrating its velocity using any forces.
*/
void physRigidBodyUpdate(physicsRigidBody *const restrict body, const float dt){
	// Apply gravity.
	const vec3 gravity = {.x = 0.f, .y = -9.80665f * body->mass, .z = 0.f};
	physRigidBodyApplyLinearForce(body, &gravity);
	// Update the body's velocity.
	physRigidBodyIntegrateVelocity(body, dt);
	physRigidBodyResetAccumulators(body);

	physRigidBodyPositionFromCentroid(body);
}


void physRigidBodyDefDelete(physicsRigidBodyDef *const restrict bodyDef){
	modulePhysicsColliderFreeArray(&bodyDef->colliders);
}

void physRigidBodyDelete(physicsRigidBody *const restrict body){
	modulePhysicsColliderFreeInstanceArray(&body->colliders);
}