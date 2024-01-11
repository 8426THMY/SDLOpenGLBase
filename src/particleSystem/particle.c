#include "particle.h"


#include "particleSystemNodeContainer.h"
#include "particleSystemNode.h"

#include "utilTypes.h"


/*
** Initialize a particle. We assume that the "prev" and
** "next" pointers are handled by the particle manager.
*/
void particlePreInit(
	particle *const restrict part,
	const particleSystemNode *const restrict node
){

	const flags_t inheritFlags = node->inheritFlags;
	// Rather than initializing the previous state, we compute
	// it using the current state during post initialization.
	// The subsystem pointer should never be NULL here.
	if(flagsAreSet(inheritFlags, PARTICLE_INHERIT_TRANSFORM)){
		const transform *const parentState = node->parent->state;

		if(flagsAreSet(inheritFlags, PARTICLE_INHERIT_POSITION)){
			part->subsys.state[0].pos = parentState[0].pos;
		}else{
			vec3InitZero(&part->subsys.state[0].pos);
		}
		if(flagsAreSet(inheritFlags, PARTICLE_INHERIT_ROTATION)){
			part->subsys.state[0].rot = parentState[0].rot;
		}else{
			quatInitIdentiy(&part->subsys.state[0].rot);
		}
		if(flagsAreSet(inheritFlags, PARTICLE_INHERIT_SCALE)){
			part->subsys.state[0].scale = parentState[0].scale;
			part->subsys.state[0].shear = parentState[0].shear;
		}else{
			vec3InitSet(&part->subsys.state[0].scale, 1.f, 1.f, 1.f);
			quatInitIdentiy(&part->subsys.state[0].shear);
		}
	}else{
		transformInit(&part->subsys.state[0].pos);
	}

	vec3InitZero(&part->linearVelocity);
	vec3InitZero(&part->angularVelocity);
	vec3InitZero(&part->netForce);
	vec3InitZero(&part->netTorque);

	#if 0
	animationDataInit(&part->animData);
	#endif

	part->lifetime = 0.f;
	part->distance = 0.f;
}

void particlePostInit(
	particle *const restrict part,
	const particleSystemNode *const restrict node
){

	const flags_t inheritFlags = node->inheritFlags;
	// Calculate the where the particle would have
	// been had it been alive on the previous frame.
	// The subsystem pointer should never be NULL here.
	if(flagsAreSet(inheritFlags, PARTICLE_INHERIT_TRANSFORM)){
		const transform *const parentState = node->parent->state;
		transform invParentState;
		transform prevParentState;
		
		if(flagsAreSet(inheritFlags, PARTICLE_INHERIT_POSITION)){
			invParentState.pos = parentState[0].pos;
			prevParentState.pos = parentState[1].pos;
		}else{
			vec3InitZero(&invParentState.pos);
			vec3InitZero(&prevParentState.pos);
		}
		if(flagsAreSet(inheritFlags, PARTICLE_INHERIT_ROTATION)){
			invParentState.rot = parentState[0].rot;
			prevParentState.rot = parentState[1].rot;
		}else{
			quatInitIdentiy(&invParentState.rot);
			quatInitIdentiy(&prevParentState.rot);
		}
		if(flagsAreSet(inheritFlags, PARTICLE_INHERIT_SCALE)){
			invParentState.scale = parentState[0].scale;
			invParentState.shear = parentState[0].shear;
			prevParentState.scale = parentState[1].scale;
			prevParentState.shear = parentState[1].shear;
		}else{
			vec3InitSet(&invParentState.scale, 1.f, 1.f, 1.f);
			quatInitIdentiy(&invParentState.shear);
			vec3InitSet(&prevParentState.scale, 1.f, 1.f, 1.f);
			quatInitIdentiy(&prevParentState.shear);
		}

		// prevState = prevParentState * curParentState^(-1) * curState
		transformInvert(&invParentState);
		transformAppendOut(&invParentState, &part->subsys.state[0], &part->subsys.state[1]);
		transformAppendP2(&prevParentState, &part->subsys.state[1]);
	}else{
		part->subsys.state[1] = part->subsys.state[0];
	}

	// Spawn and initialize this particle's children!
	{
		particleSystemNodeContainer *curContainer =
			node->container->children;
		const particleSystemNodeContainer *const lastContainer =
			&curContainer[node->container->nodeDef->numChildren];

		particleSubsysInit(&part->subsys);

		// Create instances of each of the child nodes.
		while(curContainer != lastContainer){
			particleSubsysPrepend(
				&part->subsys,
				particleSysNodeContainerInstantiate(curContainer)
			);
			++curContainer;
		}
	}
}


void particlePreUpdate(
	particle *const restrict part,
	const transform *const restrict parentState,
	const float dt
){

	// Set the particle's previous state to its current state.
	part->subsys.state[1] = part->subsys.state[0];
	// If the particle has a parent, move it to where its parent is.
	if(parentState != NULL){
		transform inversePrevState;

		// curState = curParentState * prevParentState^(-1) * prevState
		transformInvertOut(&parentState[1], &inversePrevState);
		transformAppendP2(&inversePrevState, &part->subsys.state[0]);
		transformAppendP2(&parentState[0], &part->subsys.state[0]);
	}

	// Integrate the particle's velocity using symplectic Euler.
	vec3Fmaf(dt, &part->netForce, &part->linearVelocity);
	vec3Fmaf(dt, &part->netTorque, &part->angularVelocity);
}

void particlePostUpdate(particle *const restrict part, const float dt){
	// Integrate the particle's position using symplectic Euler.
	vec3Fmaf(dt, &part->linearVelocity, &part->subsys.state[0].pos);
	quatIntegrate(&part->subsys.state[0].rot, &part->angularVelocity, dt);
	quatNormalizeQuatFast(&part->subsys.state[0].rot);

	#if 0
	// Update the texture group animation.
	texGroupStateUpdate(&part->texState, dt);
	#endif

	part->lifetime -= dt;
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