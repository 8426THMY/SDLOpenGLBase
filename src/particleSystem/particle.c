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

	const flags_t inheritFlag = flagsMask(node->flags, PARTICLE_INHERIT_MASK);
	// Rather than initializing the previous state, we compute
	// it using the current state during post initialization.
	// The parent state pointer should never be NULL here.
	if(flagsAreSet(inheritFlag, PARTICLE_INHERIT_TRANSFORM)){
		if(flagsAreSet(node->flags, PARTICLE_INHERIT_POSITION)){
			part->state[0].pos = node->parentState[0].pos;
		}else{
			vec3InitZero(&part->state[0].pos);
		}
		if(flagsAreSet(node->flags, PARTICLE_INHERIT_ROTATION)){
			part->state[0].rot = node->parentState[0].rot;
		}else{
			quatInitIdentiy(&part->state[0].rot);
		}
		if(flagsAreSet(inheritFlag, PARTICLE_INHERIT_SCALE)){
			part->state[0].scale = node->parentState[0].scale;
			part->state[0].shear = node->parentState[0].shear;
		}else{
			vec3InitSet(&part->state[0].scale, 1.f, 1.f, 1.f);
			quatInitIdentiy(&part->state[0].shear);
		}
	}else{
		transformInit(&part->state[0].pos);
	}

	vec3InitZero(&part->linearVelocity);
	vec3InitZero(&part->angularVelocity);
	vec3InitZero(&part->netForce);
	vec3InitZero(&part->netTorque);

	#if 0
	animationDataInit(&part->animData);
	#endif

	part->lifetime = 0.f;
}

void particlePostInit(
	particle *const restrict part,
	const particleSystemNode *const restrict node
){

	const flags_t inheritFlag = flagsMask(node->flags, PARTICLE_INHERIT_MASK);
	const transform *const parentState = node->group->parentState;
	// Calculate the where the particle would have
	// been had it been alive on the previous frame.
	// The group and parent state pointers should
	// never be NULL here.
	if(flagsAreSet(inheritFlag, PARTICLE_INHERIT_TRANSFORM)){
		transform invParentState;
		transform prevParentState;
		
		if(flagsAreSet(inheritFlag, PARTICLE_INHERIT_POSITION)){
			invParentState.pos = parentState[0].pos;
			prevParentState.pos = parentState[1].pos;
		}else{
			vec3InitZero(&invParentState.pos);
			vec3InitZero(&prevParentState.pos);
		}
		if(flagsAreSet(inheritFlag, PARTICLE_INHERIT_ROTATION)){
			invParentState.rot = parentState[0].rot;
			prevParentState.rot = parentState[1].rot;
		}else{
			quatInitIdentiy(&invParentState.rot);
			quatInitIdentiy(&prevParentState.rot);
		}
		if(flagsAreSet(inheritFlag, PARTICLE_INHERIT_SCALE)){
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
		transformAppendOut(&invParentState, &part->state[0], &part->state[1]);
		transformAppendP2(&prevParentState, &part->state[1]);
	}else{
		part->state[1] = part->state[0];
	}

	// Spawn and initialize this particle's children!
	{
		particleSystemNodeContainer *curContainer =
			node->container->children;
		const particleSystemNodeContainer *const lastContainer =
			&curContainer[node->container->nodeDef->numChildren];

		particleSysNodeGroupInit(&part->group, part->state);

		// Create instances of each of the child nodes.
		while(curContainer != lastContainer){
			particleSysNodeGroupPrepend(
				&part->group,
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
	part->state[1] = part->state[0];
	// If the particle has a parent, move it to where its parent is.
	if(parentState != NULL){
		transform inversePrevState;

		// curState = curParentState * prevParentState^(-1) * prevState
		transformInvertOut(&parentState[1], &inversePrevState);
		transformAppendP2(&inversePrevState, &part->state[0]);
		transformAppendP2(&parentState[0], &part->state[0]);
	}

	// Integrate the particle's velocity using symplectic Euler.
	vec3Fmaf(dt, &part->netForce, &part->linearVelocity);
	vec3Fmaf(dt, &body->netTorque, &body->angularVelocity);
}

void particlePostUpdate(particle *const restrict part, const float dt){
	// Integrate the particle's position using symplectic Euler.
	vec3Fmaf(dt, &body->linearVelocity, &body->state[0].pos);
	quatIntegrate(&body->state[0].rot, &body->angularVelocity, dt);
	quatNormalizeQuatFast(&body->state[0].rot);

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
	particleSysNodeGroupOrphan(&part->group);
}