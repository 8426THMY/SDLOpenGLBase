#include "program.h"


/**
Solve velocity constraints.
Integrate positions.
Solve positions constraints.
Update centroids and inertia tensors. (?)
**/

/**
Game logic.
Update objects.
Update skeletons.
Update physics objects.

Bones that physics objects are attached to should move during physics object update.
Otherwise, the mesh will never reflect changes made to physically simulated bones.
How can we do this?
**/
#warning "If collisions are acting funny, it might be because quatRotateVec3Fast is derping. For some reason, it seems to be inaccurate in very rare cases."
#warning "Not sure if we get 1/dt in our contact functions, either."
#warning "Differentiate between joints and constraints. Constraints are always between two physics objects and joints often have an anchor point."

#warning "Do we extend the total memory when we run out? Maybe make it so this functionality can be toggled."

#warning "Add more matrix functions for left- and right-multiplying vectors by matrices. We need bra and ket functions that produce a vector on left-multiplication."
#warning "Should we use our 'R' math functions? It would also be a good idea to stop passing so many things by pointer."

#warning "Add 'const restrict' keywords to pointers where applicable? That's a lot of work though."
#warning "Change 'const type *' to 'const type *const' almost everywhere we use it."

#warning "Texture group animation needs a rewrite. It's messy and we can't do diagonal scrolling effects."
#warning "Model loading also needs a rewrite, but it's not really important since we won't need it when we have a binary format."

#warning "Should resource arrays be sorted? Then we could use a binary search to find particular items by name."
#warning "What if we just load everything in alphabetical order? Is that possible?"

#warning "When we implement intrinsics, use '_MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON)' to better handle subnormal numbers."
#warning "Apparently '-ffast-math' should do this anyway. Maybe test it?"
/* https://stackoverflow.com/questions/9314534/why-does-changing-0-1f-to-0-slow-down-performance-by-10x */


int main(int argc, char **argv){
	program prg;

	// If we're able to initialize the libraries, setup the program and start the loop!
	if(programInit(&prg)){
		programLoop(&prg);
	}

	programClose(&prg);


	return(0);
}


// #error "Go through your functions and give them clearer descriptions."
// #error "It would probably be a good idea to try and clean up your code while you're at it."
// #error "Split some of your functions up into helpers or something too, especially in areas where the helper might be useful in the future."
// #error "Replace memsets on non-integer types with a loop."
// #error "Should we use copies for mathematics functions? We also do some unnecessary copies with matrix multiplications."
// #error "Rigid body damping, layers and flags."


/**
New To-Do List:

1. Finish the skeletal animation system on the CPU.
	a. Animate and blend skeletal animations correctly.
	b. You may have to change it to work better with update state interpolation.
	   this would involve making it interpolate between the current and next frames
	   rather than the current state and the next frame. This might be hard with blending.
	c. Skeleton and animation loading.
	d. Write a program to convert SMDs to my model and skeletal animation formats.

2. Skeletal animation on the GPU.
	a. Models should send their bone data to the GPU when they're drawing.
	b. Models are transformed on the GPU.

3. Create an entity class.
	a. Entities point to the models, textureGroups, skeletons and animations they use.
	b. Contain functions for regular transformations.

4. Update-update interpolation.
	a. It should also store data for a variable number of states in the past.
	b. Must be able to switch to them and interpolate between them at will.
	c. While interpolating through animations, do not skip frames.
	d. You don't need to store information for objects that no longer exist.
	e. If an object teleports, you need to be able to handle that. Maybe have a
	   char that indicates whether or not the object teleported or moved smoothly
	   from its last position to its current one. You could also use a float to
	   indicate when it teleported.

5. Storing models, textures, textureGroups and skeletal animations.
	a. You need a faster way of searching through them.
	b. Working on a better memory management system. This won't solve the searching problem, though.

6. Console command system.
	a. Use a trie for this.

7. Conditional rendering system.
	a. The current plan is to have scenes, which are groups of areas that
	   can be rendered by a camera. An area is a collection of objects that
	   are all rendered together. Think of them as areas in Source maps.
	b. Some scenes, such as the one used for the HUD, will only have one area.
	c. Areas have an array list allocator that stores pointers to all of the
	   objects they use. These objects are stored in the global objects array.

8. Physics engine.
	a. Implement joints and alternative collider types.
	b. Need to fix render objects so this can be tested.
	c. Create new object type whose base structure has arrays of render objects,
	   colliders and joints as well as other information currently stored by render
	   objects (such as skeletons).
**/


/** To-do **/
/*
1. Use 'f' to define textureGroup frames outside of an animation. Only used frames are kept.
2. When 't' is encountered, store the image paths in a vector. Only used textures are loaded.
3. Indexing heavily impacts loading times. See below for solution.
4. The framerate limiter is unpredictable and unreliable.
5. Maybe make the framerate a global variable for things like animated textures?
6. Need to make the texture animating code independant of the drawing function.
7. If a model uses multiple textures, either split it up into multiple objects or use multiple texture mapping units.
8. Bone animation.
9. Move the texture updating code into update rather than render.
10. Investigate "timeBeginPeriod()" and "timeEndPeriod()" for capping the framerate. Does "SDL_Delay()" use them? How do you do it on other platforms?
*/


/*
Here's what a binary model format would need to store:

vp x y z    // Vertex positions.
vt u v      // Vertex texture coordinates.
vn x y z    // Vertex normals.

v vp vt vn  // Unique vertex, where "vp", "vt" and "vn" are the data indices.

f v v v ... // Face, where "v" is the index for a unique vertex.
*/


/*
Doing bone animation:

In vertexShader.gls, you load the positions of each vertex. Perhaps
you could also load in which bones the vertices are attached to
and perform the calculations in the shader?
*/


/*
Doing the console display:

char **lineStrings[CONSOLE_MAX_LINES];
uint32_t currentLines;
uint32_t startLine;

"lineStrings" stores the text for each line we're displaying.
"currentLines" stores how many of the elements in that array are being used.
"startLine" indicates which element contains the first line to print.

When the array is full, we start overwriting elements from the beginning and
increasing startLine. We start printing from startLine and loop back to 0 when
we reach the end. When startLine reaches CONSOLE_MAX_LINES, set it back to 0.
*/


/*
Doing a particle system:

You could store a vector of each particle that is loaded and, for each of
those, a vector of instances. That way, you would only have to bind the
texture once per particle. Except that wouldn't work well because particles
will likely be using different textures. What about instancing, though?
*/


/*
keyState keyStates[NUM_KEYS];

typedef struct keyState {
	// 0  - Not pressed
	// 1  - Pressed this update
	// >1 - Held
	byte_t state;
} keyState;

void pollKeyboard(){
	size_t i;
	for(i = 0; i < NUM_KEYS; ++i){
		byte_t newState = pollKey(i);
		if(newState == 0){
			keyState[i] == newState;
		}else if(keyState[i] < 2){
			++keyState[i];
		}
	}
}


keyBind binds[NUM_KEYS];

typedef struct keyBind {
	unsigned int *ids;
	byte_t **inputs;

	byte_t numCommands;
} keyBind;
*/