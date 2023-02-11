#ifndef particleSystem_h
#define particleSystem_h


#include <stddef.h>

#include "transform.h"
#include "camera.h"

#include "particleSystemNodeGroup.h"


/**
*** We use a module allocator for particleSystemNodeDefs, so how can
*** we store them in the particleSystemDef? We probably won't be using
*** these for the containers, so how can we handle those?
**/


/*
** The particle system is rather complicated and has a lot of moving parts,
** so this comment block will attempt to briefly outline how it's laid out.
**
**
** particleSystemDef
** -----------------
** A particleSystemDef contains all the information required to spawn
** instances of a particle system. It contains a memorySingleList of
** particleSystemNodeDefs. In this list, siblings are stored next to
** each other, and always somewhere after their parents.
**
** Suppose we have the following tree structure for our node definitions:
**
**                  particleSystemDef
**                     ___/ | \___
**                    /     |     \
**                   A      B      C
**                  / \     |     / \
**                 /   \    |    /   \
**                A1   A2   B1  C1   C2
**             _/ | \_                |
**            /   |   \               |
**           A11 A12  A13            C21
**
** In this case, a valid list structure would be as follows:
**
**     A -> B -> C -> A1 -> A2 -> A11 -> A12 -> A13 -> B1 -> C1 -> C2 -> C21.
**
** Note that this isn't quite breadth-first order, as all of the
** A subtree is stored before the rest of the B and C subtrees.
**
** Because siblings are stored in order, parents need only store the
** number of children they have and a pointer to their first child.
** Further, this makes it easier when iterating through the list, as
** we will never reach a child node before its parent. In the future,
** it might be better to store the index of their first child instead.
** This would allow us to have the nodes stored breadth-first.
**
**
** particleSystemNodeDef
** ---------------------
** A particleSystemNodeDef stores all of the information required to
** spawn and update instances of a particular node in the tree. Note
** that because of how we store the list of node definitions, they
** need only store the number of children they have and a pointer to
** the first child.
**
** These structures also contain all of the initializers, operators
** and so on that are used when updating each of the instances.
**
**
** particleSystem
** --------------
** Instance of a particleSystemDef. Stores a statically-allocated array
** of particleSystemNodeContainers, one for each particleSystemNodeDefs.
** In fact, these containers are internally stored in the same order as
** the definition's list of node definitions. Not only does this make it
** easier to store each container's list of children, it also means we
** can iterate through them without having to worry about whether or not
** their parents have been updated first.
**
** Another benefit to our method of storing containers is that they're
** much easier to recursively initialize. This is why we don't quite use
** breadth-first order!
**
**
** particleSystemNodeContainer
** ---------------------------
** Stores every instance of a particular particleSystemNode that has
** been spawned in a memorySingleList. Because all of the instances
** are stored together, it means we can render them all simultaneously.
** This mainly helps reduce draw calls, but it has other perks, too.
**
** Because particles can spawn nodes lower down in the tree, we need
** to update parent nodes (and their particles) before their children.
** This method of storage ensures that simply by iterating through the
** list of containers in order, this condition is met. Thus, particles
** don't need to update the nodes they spawn, since their containers
** will always be updated later on. This makes it easier to handle the
** case when a particle dies before its children.
**
**
** particleSystemNodeGroup
** -----------------------
** Stores a doubly-linked list of particleSystemNodes that all share
** the same parent. This structure is used by anything that is capable
** of spawning particle system nodes. It contains a list of all of the
** parent's children as well as a pointer to the parent's transformation.
**
** We allow particles to inherit the transformations of their parents,
** we note that parent particles need to know about their children so
** they can update the parent transformations. We store pointers to
** these parent transformations, so we only need to update them twice:
** once when the parent is spawned and again when it is deleted. This
** coincides with when we need to update the group, so we store them
** together.
**
** Note that unless we have child nodes store copies of their parent's
** transformations, children need to know about their parents, too.
** Hence, by having them store pointers to the groups they live in,
** we can give them knowledge of their parent's transformations. When
** the parent dies, it sets the group pointers of its children to NULL,
** which informs the children that there is nothing to inherit.
**
**
** particleSystemNode
** ------------------
** Instance of a particleSystemNodeDef. This structure is perhaps the
** most important of the particle system, as it is responsible for most
** of the logic involved in spawning and controlling particles.
**
** Although most of the particle controllers require no additional memory,
** and can hence simply be stored by the definition, emitters are the one
** exception. This is because they typically need to keep track of how
** many particles they've emitted (in the last second, for example).
**
** Nodes also store the linked list pointers for the groups they live in.
** They also store a reference to the containers that they instantiate.
** These are used to obtain a reference to the node definition, and also
** define the containers that any child nodes should be in.
**
**
** particleManager
** ---------------
** Stores a statically-allocated array of particles, with size given
** by the "maxParticles" parameter of the owner node's definition.
** This array is treated as a free list, which allows us to instantly
** allocate and delete particles without any sort of iteration.
**
** This structure is also responsible for sorting particles, and hence
** operating the "prev" and "next" pointers that they store. However,
** in order to correctly sort particles, managers will require hints
** of the sorting order from the particle system nodes that own them.
**
**
** particle
** --------
** Stores all the parameters that a particle might require. As mentioned
** above, particles are responsible for spawning particle system nodes
** for the next level down. Hence, particles store a particle system group.
**
** Particles store their previous and current global transformations. In
** order to use many of the operators and constraints (including collision
** detection), we need their global transformations. However, we can only
** get the next global transformation from the previous one if we know the
** the previous and current global transformations of the parent. Hence,
** particles must store both their previous and current transformations.
**
** In order to facilitate sorting, particles also store pointers to the
** particles that come before and after them. These pointers are always
** handled by the particle manager.
*/


typedef struct particleSystemNodeDef particleSystemNodeDef;
typedef struct particleSystemDef {
	char *name;
	// Single list of node definitions. Note that siblings
	// are stored sequentially in the list, and always come
	// somewhere after their parent. This makes it easier
	// to instantiate particle systems.
	particleSystemNodeDef *nodes;
	size_t numNodes;
	// Number of root-level nodes. We store this so we
	// don't have to figure it out when instantiating.
	size_t numRoot;
} particleSystemDef;

/*
** A particle system is essentially just a tree of subsystems.
** Note that we store an array of all of the nodes in the tree
** sorted by texture name: because the textures don't change
** during runtime, we can use this ordering to reduce texture
** bindings and hence draw calls.
*/
typedef struct particleSystemNodeContainer particleSystemNodeContainer;
typedef struct particleSystem {
	// Stores the current and previous configurations of the
	// particle system. This is typically updated by the owner.
	// Depending on the inheritance settings, we can either:
	//     1. Ignore this (never inherit).
	//     2. Add this to newly-created particles (inherit on create).
	//     3. Add this to particles when rendering (always inherit).
	// The owner is responsible for updating this.
	transform state[2];
	// Particle system node containers are stored such that
	// the children of a particular container are sequential.
	// This just makes the memory a bit easier to manage.
	particleSystemNodeContainer *containers;
	// This actually points to the address after the last container.
	particleSystemNodeContainer *lastContainer;
	// List of root-level nodes. These are only spawned
	// when the particle system is first initialized.
	particleSystemNodeGroup group;
} particleSystem;


void particleSysDefInit(particleSystemDef *const restrict partSysDef);
void particleSysInit(
	particleSystem *const restrict partSys,
	const particleSystemDef *const restrict partSysDef,
	const transformState *const restrict state
);

void particleSysUpdate(particleSystem *const restrict partSys, const float dt);
void particleSysDraw(
	const particleSystem *const restrict partSys,
	const camera *const restrict cam, const float dt
);

void particleSysDelete(particleSystem *const restrict partSys);
void particleSysDefDelete(particleSystemDef *const restrict partSysDef);


#endif