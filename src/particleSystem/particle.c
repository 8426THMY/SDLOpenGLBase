#include "particle.h"


#include "particleSystemNodeContainer.h"
#include "particleSystemNode.h"

#include "utilTypes.h"


// Initialize a particle.
void particleInit(
	particle *const restrict part,
	const particleSystemNodeContainer *const restrict children,
	const size_t numChildren
){

	transformInit(&part->localState);
	transformInit(&part->state);
	transformInit(&part->prevState);

	vec3InitZero(&part->linearVelocity);
	vec3InitZero(&part->angularVelocity);
	vec3InitZero(&part->netForce);
	vec3InitZero(&part->netTorque);

	#if 0
	animationDataInit(&part->animData);
	#endif

	part->lifetime = 0.f;

	particleSubsysInstantiate(part->subsys, children, numChildren, part);
}


void particlePreUpdate(particle *const restrict part, const float dt){
	// Integrate the particle's velocity using symplectic Euler.
	vec3Fmaf(dt, &part->netForce, &part->linearVelocity);
	vec3Fmaf(dt, &part->netTorque, &part->angularVelocity);
}

void particlePostUpdate(particle *const restrict part, const float dt){
	// Integrate the particle's position using symplectic Euler.
	vec3Fmaf(dt, &part->linearVelocity, &part->localState.pos);
	quatIntegrate(&part->localState.rot, &part->angularVelocity, dt);
	quatNormalizeQuatFast(&part->localState.rot);

	#if 0
	// Update the texture group animation.
	texGroupStateUpdate(&part->texState, dt);
	#endif

	part->lifetime -= dt;
}

/*
** After updating the particle's local state,
** we need to append the parent's state to get
** its current and previous global transforms.
*/
void particleUpdateGlobalTransform(
	particle *const restrict part,
	const transform *const restrict curParentState,
	const transform *const restrict prevParentState
){

	transformMultiplyOut(curParentState,  &part->localState, &part->state);
	transformMultiplyOut(prevParentState, &part->localState, &part->prevState);
}

return_t particleDead(const particle *const restrict part){
	return(part->lifetime <= 0.f);
}


/*
** Delete a particle. Note that the "prev" and "next"
** pointers must be handled by the particle manager.
*/
void particleDelete(particle *const restrict part){
	particleSubsysOrphan(&part->subsys);
}