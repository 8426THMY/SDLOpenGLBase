#include "physicsJointSphere.h"


#include "physicsRigidBody.h"


/*
** ----------------------------------------------------------------------
**
** Spherical constraints involve a "point-to-point" constraint and
** a cone constraint that limits one rigid body's relative position
** and orientation to a cone about some user-specified axis.
** This constraint is similar to a ball and socket joint.
**
** ----------------------------------------------------------------------
**
** C1 : (pA + RA*aA) - (pB + RB*aB) = 0,
**
** where pA, pB are the positions of bodies A and B, RA and RB are their
** orientations and aA and aB are the vectors from bodies A and B to the
** anchor point.
**
** How would we limit the angles for each axis?
*/
/**
(pA + RA*aA) - (pB + RB*aB) = 0
(vA + cross(wA, RA*aA)) - (vB + cross(wB, RB*aB)) = 0
vA - cross(RA*aA, wA) - vB + cross(RB*aB, wB) = 0
vA - crossmat(RA*aA)*wA - vB + crossmat(RB*aB)*wB = 0

JV = 0

    [vA]
    [wA]
V = [vB],
    [wB]

J = [I, -crossmat(RA*aA), -I, crossmat(RB*aB)].


JM^(-1) = [mA^(-1), -crossmat(RA*aA)*IA^(-1), -mB^(-1), crossmat(RB*aB)*IB^(-1)]
JM^(-1)J^T = (mA^(-1) + mB^(-1))*I - crossmat(RA*aA)*IA^(-1)*crossmat(RA*aA) - crossmat(RB*aB)*IB^(-1)*crossmat(RB*aB)

Note that the effective mass is a symmetric 3x3 matrix.
**/


/*
** Calculate any values required by collision resolution
** that are not expected to change between iterations.
** Such values include the effective mass and the bias.
*/
void physJointSpherePresolve(
	void *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB, const float dt
){

	//
}

/*
** Apply an impulse to the physics objects to make sure
** they are within the constraints imposed by the joint.
** This may be called multiple times with sequential impulse.
*/
void physJointSphereSolveVelocity(void *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB){
	//
}

/*
** When we're using full non-linear Gauss-Seidel, we apply
** an impulse directly to the rigid bodies' positions.
** This may also be called multiple times, but by returning
** the amount of error we'll know when to stop.
*/
#ifdef PHYSJOINTSPHERE_STABILISER_GAUSS_SEIDEL
return_t physJointSphereSolvePosition(void *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB){
	return(1);
}
#endif