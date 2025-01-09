#include "particle.h"


#include "particleSystemNodeContainer.h"
#include "particleSystemNode.h"

#include "utilTypes.h"


// Initialize a particle.
void particleInit(
	particle *const restrict part,
	const particleSystemNode *const restrict node
){

	particleSystemNodeContainer *curContainer =
		node->container->children;
	const particleSystemNodeContainer *const lastContainer =
		&curContainer[node->container->nodeDef->numChildren];

	transformInit(&part->localState);
	particleSubsysInit(&part->subsys);

	// Create instances of each of the child nodes.
	while(curContainer != lastContainer){
		particleSubsysPrepend(
			&part->subsys,
			particleSysNodeContainerInstantiate(curContainer)
		);
		++curContainer;
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

/*
** This is called after updating the freshly-initialized
** particle with all of the emitters. We need to update
** its global transform using its parent and spawn any
** instances of child particle system nodes.
*/
/**void particlePostInit(
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
}**/


/**
previousGlobalState;
currentGlobalState;
currentLocalState;


Updating:  Need current and previous local transforms to update from the parent transforms
           properly, and to handle teleporting parents correctly. We also need the current
           and previous global transforms so that particles can inherit from their parents
           and so that we can presort particles before rendering.
Rendering: Need current and previous global transforms.

Unfortunately, it looks like we need all four states...
This makes things harder when particles become orphaned, since the only way to get
the old parent's state is to remove the local transform from the global one. If the
local transform isn't invertible (say the scale is 0), then we've totally lost the
parent's state! This may be why particles in Effekseer store their parent's states.
The problem is that then we probably need to store five transforms... ugh...

Actually, let's think about this properly.
	1. Update.
		a. To compute the previous global state, we need the
		   previous parent state and the current local state
		   prior to being updated this tick.
		b. To compute the current global state, we need the
		   current parent state and the current local state
		   after being updated this tick.
		c. To presort the particles, we need the current
		   global state.
	2. Render.	
		a. To compute the interpolated global state, we
		   need the previous and current global states.
Note that while we can skip the previous local state by just using
the current one before updating it, we can't play the same game
with the previous parent state. The reason for this is, as you might
expect, teleporting. It looks like we'll probably need the following:
	1. Parent state pointer (for previous and current parent states).
	2. Current parent state (for when the parent pointer is NULL).
	3. Previous global state.
	4. Current global state.
Also, it looks like I'm not doing this, so to be totally clear:
the parent state should be applied after the local state!!

The most memory efficient way to do this would be to use a special
allocator for subsystems. Unfortunately, this means that particles'
states could be who-knows-where in memory when it's time to update.
The alternative is to have particleSystemNodes store copies of their
parents' states and hope that it doesn't get uncached when updating
each particle in the node.


update {
	previousGlobalState = currentLocalState*previousParentState;
	currentGlobalState  = currentLocalState*currentParentState;
	// We need to remember the inverse of the parent's current
	// state so we can recover the updated local state later on.
	currentLocalState   = currentParentState^{-1};

	// update currentGlobalState

	currentLocalState = currentGlobalState*currentLocalState^{-1};
}
**/
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

	transformMultiplyOut(curParentState,  &part->localState, &part->subsys.state[0]);
	transformMultiplyOut(prevParentState, &part->localState, &part->subsys.state[1]);
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