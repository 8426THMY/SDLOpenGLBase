#include "physicsRigidBody.h"


#include <stdio.h>

#include "utilString.h"
#include "modulePhysics.h"


#define PHYSRIGIDBODYDEF_PATH_PREFIX        ".\\resource\\physics\\"
#define PHYSRIGIDBODYDEF_PATH_PREFIX_LENGTH (sizeof(PHYSRIGIDBODYDEF_PATH_PREFIX) - 1)


void physRigidBodyDefInit(physicsRigidBodyDef *bodyDef){
	bodyDef->colliders = NULL;
}

void physRigidBodyInit(physicsRigidBody *body, const physicsRigidBodyDef *bodyDef){
	body->colliders = /** ALLOCATE NEW COLLIDER LIST **/NULL;

	body->mass = bodyDef->mass;
	body->invMass = bodyDef->mass;

	body->centroidLocal = bodyDef->centroid;
	body->invInertiaLocal = bodyDef->invInertia;

	transformStateInit(&body->transform);

	vec3InitZero(&body->linearVelocity);
	vec3InitZero(&body->angularVelocity);
	vec3InitZero(&body->netForce);
	vec3InitZero(&body->netTorque);
}


// Load a rigid body, including any of its colliders.
return_t physRigidBodyDefLoad(physicsRigidBodyDef *bodyDef, const char *bodyPath){
	physRigidBodyDefInit(bodyDef);


	// Find the full path for the rigid body!
	const size_t bodyPathLength = strlen(bodyPath);
	char *bodyFullPath = memoryManagerGlobalAlloc(PHYSRIGIDBODYDEF_PATH_PREFIX_LENGTH + bodyPathLength + 1);
	if(bodyFullPath == NULL){
		/** MALLOC FAILED **/
	}
	memcpy(bodyFullPath, PHYSRIGIDBODYDEF_PATH_PREFIX, PHYSRIGIDBODYDEF_PATH_PREFIX_LENGTH);
	strcpy(bodyFullPath + PHYSRIGIDBODYDEF_PATH_PREFIX_LENGTH, bodyPath);

	// Load the rigid body!
	FILE *bodyFile = fopen(bodyFullPath, "r");
	if(bodyFile != NULL){
		return_t success = 0;


		physicsCollider *curCollider = NULL;
		float curMass;
		vec3 curCentroid;
		mat3 curInertia;
		byte_t colliderLoaded;

		char *tokPos;

		char lineBuffer[1024];
		char *line;
		size_t lineLength;

		while((line = readLineFile(bodyFile, &lineBuffer[0], &lineLength)) != NULL){
			if(curCollider != NULL){
				// New collider.
				if(memcmp(line, "c ", 2) == 0 && line[lineLength - 1] == '{'){
					// Load the collider using its respective function.
					colliderLoaded = colliderLoad(&curCollider->global, bodyFile, &curCentroid, &curInertia);

				// Physics collider mass.
				}else if(memcmp(line, "m ", 2) == 0){
					curMass = strtod(&line[2], &tokPos);
					// If the mass specified is invalid, use the default value.
					if(&line[2] == tokPos){
						curMass = PHYSCOLLIDER_DEFAULT_MASS;
					}

				// Physics collider density.
				}else if(memcmp(line, "d ", 2) == 0){
					curCollider->density = strtod(&line[2], &tokPos);
					// If the density specified is invalid, use the default value.
					if(&line[2] == tokPos){
						curCollider->density = PHYSCOLLIDER_DEFAULT_DENSITY;
					}

				// Physics collider friction.
				}else if(memcmp(line, "f ", 2) == 0){
					curCollider->friction = strtod(&line[2], &tokPos);
					// If the friction specified is invalid, use the default value.
					if(&line[2] == tokPos){
						curCollider->friction = PHYSCOLLIDER_DEFAULT_FRICTION;
					}

				// Physics collider restitution.
				}else if(memcmp(line, "r ", 2) == 0){
					curCollider->restitution = strtod(&line[2], &tokPos);
					// If the restitution specified is invalid, use the default value.
					if(&line[2] == tokPos){
						curCollider->restitution = PHYSCOLLIDER_DEFAULT_RESTITUTION;
					}

				// Physics collider end.
				}else if(line[0] == '}'){
					// Add the new physics collider to our rigid body!
					if(colliderLoaded){
						physRigidBodyDefAddCollider(bodyDef, curMass, &curCentroid, &curInertia);
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
					char *boneName;
					size_t boneNameLength;
					// Find the name of the bone to attach this rigid body to!
					boneName = getMultiDelimitedString(&line[2], lineLength - 2, "\" ", &boneNameLength);

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
							modulePhysicsColliderPrepend(&bodyDef->colliders);
							curCollider = bodyDef->colliders;
							curMass = PHYSCOLLIDER_DEFAULT_MASS;
							physColliderInit(curCollider, currentType, bodyDef);

							// When we actually load this collider successfully, we can
							// set this value. This exists to make sure we don't add a
							// collider to a rigid body without loading it properly.
							colliderLoaded = 0;

						// If an invalid type was specified, ignore the collider.
						}else{
							printf(
								"Error loading rigid body!\n"
								"Path: %s\n"
								"Line: %s\n"
								"Error: Ignoring invalid collider of type '%u'.\n",
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
		memoryManagerGlobalFree(bodyFullPath);


		// If we loaded at least one
		// collider, keep the rigid body!
		if(success){
			#warning "Do something with the bone name, perhaps?"

		// Otherwise, delete the rigid body.
		}else{
			physRigidBodyDefDelete(bodyDef);
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
** Assuming "centroidArray" is an array of collider centroids,
** combine them all to get the rigid body's centroid.
** We assume that they have already been weighted.
*/
void physRigidBodyDefSumCentroids(physicsRigidBodyDef *bodyDef, const vec3 *centroidArray, const size_t numBodies){
	const vec3 *lastCentroid = &centroidArray[numBodies];
	vec3 tempCentroid;

	vec3InitZero(&tempCentroid);

	// Store the sum of the weighted centroids in "tempCentroid".
	while(centroidArray != lastCentroid){
		vec3AddVec3(&tempCentroid, centroidArray);

		++centroidArray;
	}

	vec3MultiplySOut(&tempCentroid, bodyDef->invMass, &bodyDef->centroid);
}

/*
** Assuming "inertiaArray" is an array of collider inertia tensors,
** combine them all to get the rigid body's moment of inertia tensor.
** We assume that they have already been weighted.
*/
void physRigidBodyDefSumInertia(physicsRigidBodyDef *bodyDef, const mat3 *inertiaArray, const size_t numBodies){
	const mat3 *lastInertia = &inertiaArray[numBodies];
	float tempInertia[6];

	memset(tempInertia, 0.f, sizeof(tempInertia));

	// Store the sum of the weighted centroids in "tempCentroid".
	while(inertiaArray != lastInertia){
		tempInertia[0] += inertiaArray->m[0][0];
		tempInertia[1] += inertiaArray->m[1][1];
		tempInertia[2] += inertiaArray->m[2][2];
		tempInertia[3] += inertiaArray->m[0][1];
		tempInertia[4] += inertiaArray->m[0][2];
		tempInertia[5] += inertiaArray->m[1][2];

		++inertiaArray;
	}

	bodyDef->invInertia.m[0][0] = tempInertia[0];
	bodyDef->invInertia.m[1][1] = tempInertia[1];
	bodyDef->invInertia.m[2][2] = tempInertia[2];
	bodyDef->invInertia.m[0][1] = tempInertia[3];
	bodyDef->invInertia.m[0][2] = tempInertia[4];
	bodyDef->invInertia.m[1][2] = tempInertia[5];
	// These should be the same as the values we've already calculated.
	bodyDef->invInertia.m[1][0] = tempInertia[3];
	bodyDef->invInertia.m[2][0] = tempInertia[4];
	bodyDef->invInertia.m[2][1] = tempInertia[5];

	// The inverse inertia tensor is more useful to us than the regular one.
	mat3Invert(&bodyDef->invInertia);
}

/*
** Adds a physics collider to a rigid body's list. This
** assumes that the collider's centroid and moment of
** inertia tensor have already been calculated. Note
** that this assumes that the body's inertia tensor has
** note yet been inverted. It will not invert it, either.
*/
void physRigidBodyDefAddCollider(physicsRigidBodyDef *bodyDef, const float mass, const vec3 *centroid, const mat3 *inertia){
	vec3 tempCentroid;
	vec3 tempCentroidWeighted;
	vec3 tempCentroidSquaredWeighted;
	mat3 tempInertia = *inertia;


	// Calculate the collider's contribution to the body's centroid.
	vec3MultiplySOut(centroid, mass, &tempCentroid);
	vec3MultiplyS(&bodyDef->centroid, bodyDef->mass);

	// Calculate the new mass and inverse mass.
	bodyDef->mass += mass;
	bodyDef->invMass = 1.f / bodyDef->mass;

	// Add this collider's contribution to the centroid.
	vec3AddVec3(&bodyDef->centroid, &tempCentroid);
	vec3MultiplyS(&bodyDef->centroid, bodyDef->invMass);


	/*
	** Add this collider's contribution to the inertia tensor.
	** We use the parallel axis theorem to translate it from
	** the collider's centroid to the rigid body's centroid.
	**
	** J = I + m * (dot(R, R) * E - out(R, R))
	**
	** J = new inertia tensor
	** I = old inertia tensor
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
	** dI = [0 d 0]
	**      [0 0 d]
	**
	**                 [xx yx zx]
	** P = out(R, R) = [xy yy zy]
	**                 [xz yz zz]
	**
	**          [(yy + zz)    -yx       -zx   ]
	**          [   -xy    (xx + zz)    -zy   ]
	** dI - P = [   -xz       -yz    (xx + yy)]
	**
	** J = I + m * (dI - P)
	*/

	vec3SubtractVec3FromOut(&bodyDef->centroid, centroid, &tempCentroid);
	vec3MultiplySOut(&tempCentroid, mass, &tempCentroidWeighted);
	vec3MultiplyVec3Out(&tempCentroid, &tempCentroidWeighted, &tempCentroidSquaredWeighted);

	// Translate the inertia tensor using the rigid body's centroid.
	// (yy + zz) * m
	tempInertia.m[0][0] += tempCentroidSquaredWeighted.y + tempCentroidSquaredWeighted.z;
	// xy * m
	tempInertia.m[0][1] -= tempCentroid.x * tempCentroidWeighted.y;
	// xz * m
	tempInertia.m[0][2] -= tempCentroid.x * tempCentroidWeighted.z;
	// (xx + zz) * m
	tempInertia.m[1][1] += tempCentroidSquaredWeighted.x + tempCentroidSquaredWeighted.z;
	// yz * m
	tempInertia.m[1][2] -= tempCentroid.y * tempCentroidWeighted.z;
	// (xx + yy) * m
	tempInertia.m[2][2] += tempCentroidSquaredWeighted.x + tempCentroidSquaredWeighted.y;

	// Add the collider's contribution to the body's inertia tensor.
	bodyDef->invInertia.m[0][0] += tempInertia.m[0][0];
	bodyDef->invInertia.m[0][1] += tempInertia.m[0][1];
	bodyDef->invInertia.m[0][2] += tempInertia.m[0][2];
	bodyDef->invInertia.m[1][0] = bodyDef->invInertia.m[0][1];
	bodyDef->invInertia.m[1][1] += tempInertia.m[1][1];
	bodyDef->invInertia.m[1][2] += tempInertia.m[1][2];
	bodyDef->invInertia.m[2][0] = bodyDef->invInertia.m[0][2];
	bodyDef->invInertia.m[2][1] = bodyDef->invInertia.m[1][2];
	bodyDef->invInertia.m[2][2] += tempInertia.m[2][2];
}

/*
** Sum the mass, centroid and intertia tensor of each of the body's
** colliders in order to find the combined centroid and inverse
** inertia tensor. This will also calculate the body's inverse mass.
** The array "masses" is an array of float arrays, each of which
** contains a collider's mass followed by any vertex weights.
*/
void physRigidBodyDefGenerateProperties(physicsRigidBodyDef *bodyDef, const float **massArrays){
	float tempMass = 0.f;
	vec3 tempCentroid;
	float tempInertia[6];
	physicsCollider *curCollider = bodyDef->colliders;
	const float **curMassArray = massArrays;

	vec3InitZero(&tempCentroid);
	memset(tempInertia, 0.f, sizeof(tempInertia));

	// Calculate the rigid body's combined mass and centroid!
	while(curCollider != NULL){
		vec3 curCentroid;
		const float curMass = (*curMassArray)[0];

		/** Convex hulls should have their vertices weighted. **/
		physColliderGenerateCentroid(curCollider, &curCentroid);

		// Add the collider's contribution to the rigid body's centroid.
		vec3MultiplyS(&curCentroid, curMass);
		vec3AddVec3(&tempCentroid, &curCentroid);
		tempMass += curMass;

		curCollider = memSingleListNext(curCollider);
		++curMassArray;
	}

	bodyDef->mass = tempMass;
	bodyDef->centroid = tempCentroid;


	if(tempMass != 0.f){
		bodyDef->invMass = 1.f / tempMass;
		vec3MultiplyS(&tempCentroid, bodyDef->invMass);


		curCollider = bodyDef->colliders;
		curMassArray = massArrays;
		// Calculate the rigid body's combined intertia tensor!
		while(curCollider != NULL){
			mat3 curInertia;

			/** Convex hulls should have their vertices weighted. **/
			physColliderGenerateInertia(curCollider, &tempCentroid, &curInertia);

			// Add the collider's contribution to the rigid body's inertia tensor.
			tempInertia[0] += curInertia.m[0][0];
			tempInertia[1] += curInertia.m[1][1];
			tempInertia[2] += curInertia.m[2][2];
			tempInertia[3] += curInertia.m[0][1];
			tempInertia[4] += curInertia.m[0][2];
			tempInertia[5] += curInertia.m[1][2];

			curCollider = memSingleListNext(curCollider);
			++curMassArray;
		}

		bodyDef->invInertia.m[0][0] = tempInertia[0];
		bodyDef->invInertia.m[1][1] = tempInertia[1];
		bodyDef->invInertia.m[2][2] = tempInertia[2];
		bodyDef->invInertia.m[0][1] = tempInertia[3];
		bodyDef->invInertia.m[0][2] = tempInertia[4];
		bodyDef->invInertia.m[1][2] = tempInertia[5];
		// These should be the same as the values we've already calculated.
		bodyDef->invInertia.m[1][0] = tempInertia[3];
		bodyDef->invInertia.m[2][0] = tempInertia[4];
		bodyDef->invInertia.m[2][1] = tempInertia[5];

		// The inverse inertia tensor is more useful to us than the regular one.
		mat3Invert(&bodyDef->invInertia);
	}else{
		bodyDef->invMass = 0.f;
		mat3InitZero(&bodyDef->invInertia);
	}
}


/*
** Calculate the body's increase in
** velocity for the current timestep.
**
** v^(t + 1) = v^n + F * m^-1 * dt
** w^(t + 1) = w^n + T * I^-1 * dt
*/
void physRigidBodyIntegrateVelocitySymplecticEuler(physicsRigidBody *body, const float dt){
	vec3 linearAcceleration;
	vec3 angularAcceleration;

	// Calculate the body's linear acceleration.
	vec3MultiplySOut(&body->netForce, body->invMass * dt, &linearAcceleration);
	// Add the linear acceleration to the linear velocity.
	vec3AddVec3(&body->linearVelocity, &linearAcceleration);

	// Calculate the body's angular acceleration.
	vec3MultiplySOut(&body->netTorque, dt, &angularAcceleration);
	mat3MultiplyByVec3(&body->invInertiaGlobal, &angularAcceleration);
	// Add the angular acceleration to the angular velocity.
	vec3AddVec3(&body->angularVelocity, &angularAcceleration);
}

/*
** Calculate the body's change in
** position for the current timestep.
**
** x^(t + 1) = x^t + v^(t + 1) * dt
** dq/dt = 0.5 * w * q
** q^(t + 1) = q^t + dq/dt * dt
*/
void physRigidBodyIntegratePositionSymplecticEuler(physicsRigidBody *body, const float dt){
	vec3 linearVelocityDelta;

	vec3MultiplySOut(&body->linearVelocity, dt, &linearVelocityDelta);
	// Compute the object's new position.
	vec3AddVec3(&body->transform.pos, &linearVelocityDelta);

	// Calculate the change in orientation.
	quatIntegrate(&body->transform.rot, &body->angularVelocity, dt);
	// Don't forget to normalize it, as
	// this process can introduce errors.
	quatNormalizeQuat(&body->transform.rot);
}


// Add a translational and rotational impulse to a rigid body.
void physRigidBodyApplyPositionalImpulse(physicsRigidBody *body, vec3 J){
	// Linear velocity.
	vec3MultiplyS(&J, body->mass);
	vec3AddVec3(&body->linearVelocity, &J);
}

// Subtract a translational impulse from a rigid body.
void physRigidBodyApplyPositionalImpulseInverse(physicsRigidBody *body, vec3 J){
	// Linear velocity.
	vec3MultiplyS(&J, body->mass);
	vec3SubtractVec3From(&body->linearVelocity, &J);
}

// Add a rotational impulse to a rigid body.
void physRigidBodyApplyAngularImpulse(physicsRigidBody *body, const vec3 *r, vec3 J){
	// Angular velocity.
	vec3CrossVec3By(r, &J);
	mat3MultiplyByVec3(&body->invInertiaGlobal, &J);
	vec3AddVec3(&body->angularVelocity, &J);
}

// Subtract a rotational impulse from a rigid body.
void physRigidBodyApplyAngularImpulseInverse(physicsRigidBody *body, const vec3 *r, vec3 J){
	// Angular velocity.
	vec3CrossVec3By(r, &J);
	mat3MultiplyByVec3(&body->invInertiaGlobal, &J);
	vec3SubtractVec3From(&body->angularVelocity, &J);
}

// Add a translational and rotational impulse to a rigid body.
void physRigidBodyApplyImpulse(physicsRigidBody *body, const vec3 *r, const vec3 *J){
	vec3 impulse;

	// Linear velocity.
	vec3MultiplySOut(J, body->mass, &impulse);
	vec3AddVec3(&body->linearVelocity, &impulse);

	// Angular velocity.
	vec3CrossVec3Out(r, J, &impulse);
	mat3MultiplyByVec3(&body->invInertiaGlobal, &impulse);
	vec3AddVec3(&body->angularVelocity, &impulse);
}

// Subtract a translational and rotational impulse from a rigid body.
void physRigidBodyApplyImpulseInverse(physicsRigidBody *body, const vec3 *r, const vec3 *J){
	vec3 impulse;

	// Linear velocity.
	vec3MultiplySOut(J, body->mass, &impulse);
	vec3SubtractVec3From(&body->linearVelocity, &impulse);

	// Angular velocity.
	vec3CrossVec3Out(r, J, &impulse);
	mat3MultiplyByVec3(&body->invInertiaGlobal, &impulse);
	vec3SubtractVec3From(&body->angularVelocity, &impulse);
}

// Add a translational and rotational (plus some bias) impulse to a rigid body.
void physRigidBodyApplyImpulseAngularBias(physicsRigidBody *body, const vec3 *r, const vec3 *J, const vec3 *b){
	vec3 impulse;

	// Linear velocity.
	vec3MultiplySOut(J, body->mass, &impulse);
	vec3AddVec3(&body->linearVelocity, &impulse);

	// Angular velocity.
	vec3CrossVec3Out(r, J, &impulse);
	mat3MultiplyByVec3(&body->invInertiaGlobal, &impulse);
	vec3AddVec3(&impulse, b);
	vec3AddVec3(&body->angularVelocity, &impulse);
}

// Subtract a translational and rotational (plus some bias) impulse from a rigid body.
void physRigidBodyApplyImpulseAngularBiasInverse(physicsRigidBody *body, const vec3 *r, const vec3 *J, const vec3 *b){
	vec3 impulse;

	// Linear velocity.
	vec3MultiplySOut(J, body->mass, &impulse);
	vec3SubtractVec3From(&body->linearVelocity, &impulse);

	// Angular velocity.
	vec3CrossVec3Out(r, J, &impulse);
	mat3MultiplyByVec3(&body->invInertiaGlobal, &impulse);
	vec3AddVec3(&impulse, b);
	vec3SubtractVec3From(&body->angularVelocity, &impulse);
}

#ifdef PHYSCOLLIDER_USE_POSITIONAL_CORRECTION
// Add a translational and rotational impulse to a rigid body's position.
void physRigidBodyApplyImpulsePosition(physicsRigidBody *body, const vec3 *r, const vec3 *J){
	vec3 impulse;
	quat tempRot;

	// Position.
	vec3MultiplySOut(J, body->mass, &impulse);
	vec3AddVec3(&body->transform.pos, &impulse);

	// Orientation.
	vec3CrossVec3Out(r, J, &impulse);
	mat3MultiplyByVec3(&body->invInertiaGlobal, &impulse);
	quatDifferentiateOut(&body->transform.rot, &impulse, &tempRot);
	quatAddVec4(&body->transform.rot, &tempRot);
	quatNormalizeQuat(&body->transform.rot);
}

// Subtract a translational and rotational impulse from a rigid body's position.
void physRigidBodyApplyImpulsePositionInverse(physicsRigidBody *body, const vec3 *r, const vec3 *J){
	vec3 impulse;
	quat tempRot;

	// Position.
	vec3MultiplySOut(J, body->mass, &impulse);
	vec3SubtractVec3From(&body->transform.pos, &impulse);

	// Orientation.
	vec3CrossVec3Out(r, J, &impulse);
	mat3MultiplyByVec3(&body->invInertiaGlobal, &impulse);
	quatDifferentiateOut(&body->transform.rot, &impulse, &tempRot);
	quatSubtractVec4From(&body->transform.rot, &tempRot);
	quatNormalizeQuat(&body->transform.rot);
}
#endif


/*
** Update a rigid body. This involves moving and
** rotating its centroid and inertia tensor, updating
** its velocity and updating all of its colliders.
*/
void physRigidBodyUpdate(physicsRigidBody *body, const float dt){
	physicsCollider *curCollider;


	/** update centroid **/
	/** update inverse inertia tensor **/
	/*
	** Note: Scaling the inverse inertia tensor may seem
	** difficult at first, but there is a solution. When
	** we're calculating a body's local inertia tensor,
	** we only need six unique numbers. These are:
	**
	** I[0] += yy + zz;
	** I[1] += xx + zz;
	** I[2] += xx + yy;
	** I[3] -= x * y;
	** I[4] -= x * z;
	** I[5] -= y * z;
	**
	** Scaling I[3], I[4] and I[5] is trivial - we simply
	** multiply each component by the product of the axes
	** scales. The other three, however, present a problem.
	**
	**
	** Luckily, it's not impossible. We can actually
	** calculate the values of xx, yy and zz summed for
	** every vertex. Note that once we calculate one,
	** we can calculate the others using a subtraction:
	**
	** xx = 0.5f * (-I[0] + I[1] + I[2]);
	**    = I[1] - zz;
	**    = I[2] - yy;
	** yy = 0.5f * ( I[0] - I[1] + I[2]);
	**    = I[0] - zz;
	**    = I[2] - xx;
	** zz = 0.5f * ( I[0] + I[1] - I[2]);
	**    = I[0] - yy;
	**    = I[1] - xx;
	**
	** With these values, we can easily scale I[0], I[1]
	** and I[2]. We need to multiply by the square of the
	** scale, as these values represent squares.
	**
	** I[0] = yy * sy^2 + zz * sz^2;
	** I[1] = xx * sx^2 + zz * sz^2;
	** I[2] = xx * sx^2 + yy * sy^2;
	**
	**
	** The only problem is that we can only apply this
	** to non-inverted inertia tensors.
	*/

	// Update the body's velocity.
	physRigidBodyIntegrateVelocitySymplecticEuler(body, dt);


	curCollider = body->colliders;
	// For every physics collider that is a part of
	// this rigid body, we will need to update its
	// base collider and its node in the broadphase.
	while(curCollider != NULL){
		physColliderUpdate(curCollider, NULL);
		curCollider = memSingleListNext(curCollider);
	}
}


void physRigidBodyDefDelete(physicsRigidBodyDef *bodyDef){
	modulePhysicsColliderFreeArray(&bodyDef->colliders);
}

void physRigidBodyDelete(physicsRigidBody *body){
	modulePhysicsColliderFreeInstanceArray(&body->colliders);
}