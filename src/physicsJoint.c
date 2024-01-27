#include "physicsJoint.h"


#include "physicsRigidBody.h"


void (*const physJointPresolveTable[PHYSJOINT_NUM_TYPES])(
	void *const restrict joint,
	physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB,
	const float dt
) = {

	physJointDistancePresolve,
	physJointFixedPresolve,
	physJointRevolutePresolve,
	physJointPrismaticPresolve,
	physJointSpherePresolve
};

void (*const physJointSolveVelocityTable[PHYSJOINT_NUM_TYPES])(
	void *const restrict joint,
	physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB
) = {

	physJointDistanceSolveVelocity,
	physJointFixedSolveVelocity,
	physJointRevoluteSolveVelocity,
	physJointPrismaticSolveVelocity,
	physJointSphereSolveVelocity
};

#ifdef PHYSJOINT_USE_POSITIONAL_CORRECTION
return_t (*const physJointSolvePositionTable[PHYSJOINT_NUM_TYPES])(
	const void *const restrict joint,
	physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB
) = {

	physJointDistanceSolvePosition,
	physJointFixedSolvePosition,
	physJointRevoluteSolvePosition,
	physJointPrismaticSolvePosition,
	physJointSphereSolvePosition
};
#endif


// Insert a joint into the rigid body's linked lists.
void physJointAdd(
	physicsJoint *const restrict joint,
	physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB
){

	joint->bodyA = bodyA;
	joint->bodyB = bodyB;


	joint->prevA = NULL;
	joint->nextA = bodyA->joints;
	// Since body A owns this joint, we can
	// insert it at the beginning of its list.
	if(bodyA->joints != NULL){
		bodyA->joints->prevA = joint;
	}
	bodyA->joints = joint;


	{
		physicsJoint *prev = NULL;
		physicsJoint *next = bodyB->joints;
		// Rigid body B is just along for the ride, so we should insert
		// this joint after the last joint whose first rigid body is body B.
		while(next != NULL && next->bodyA == bodyB){
			prev = next;
			next = next->nextA;
		}

		joint->prevB = prev;
		joint->nextB = next;
		// If a joint exists before the insertion point,
		// then our body B is the same as its body A.
		if(prev != NULL){
			prev->nextA = joint;
		}else{
			bodyB->joints = joint;
		}
		// If a joint exists after the insertion point,
		// then our body B is the same as its body B.
		if(next != NULL){
			next->prevB = joint;
		}
	}
}

#warning "We probably shouldn't just take in dt here, as we need 1/dt too."
void physJointPresolve(physicsJoint *const restrict joint, const float dt){
	physJointPresolveTable[joint->type]((void *)(&joint->data), joint->bodyA, joint->bodyB, dt);
}

void physJointSolveVelocity(physicsJoint *const restrict joint){
	physJointSolveVelocityTable[joint->type]((void *)(&joint->data), joint->bodyA, joint->bodyB);
}

#ifdef PHYSJOINT_USE_POSITIONAL_CORRECTION
return_t physJointSolvePosition(const physicsJoint *const restrict joint){
	return(physJointSolvePositionTable[joint->type]((void *)(&joint->data), joint->bodyA, joint->bodyB));
}
#endif



/*
** Delete a joint and remove it from the linked
** lists of both colliders involved in the joint.
*/
void physJointDelete(physicsJoint *const restrict joint){
	physicsJoint *prev = joint->prevA;
	physicsJoint *next = joint->nextA;

	// The previous joint in body A's
	// list should point to the next one.
	if(prev != NULL){
		if(prev->bodyA == joint->bodyA){
			prev->nextA = next;
		}else{
			prev->nextB = next;
		}
	}else{
		joint->bodyA->joints = next;
	}
	// The next joint in body A's list
	// should point to the previous one.
	if(next != NULL){
		if(next->bodyA == joint->bodyA){
			next->prevA = prev;
		}else{
			next->prevB = prev;
		}
	}

	next = joint->nextB;
	prev = joint->prevB;
	// Do the same again but for body B.
	if(prev != NULL){
		if(prev->bodyB == joint->bodyB){
			prev->nextB = next;
		}else{
			prev->nextA = next;
		}
	}else{
		joint->bodyB->joints = next;
	}
	if(next != NULL){
		if(next->bodyB == joint->bodyB){
			next->prevB = prev;
		}else{
			next->prevA = prev;
		}
	}
}