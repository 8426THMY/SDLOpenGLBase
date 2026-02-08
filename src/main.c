#include "program.h"


#warning "Differentiate between joints and constraints. Constraints are always between two physics objects and joints often have an anchor point."

#warning "Should we use our 'R' math functions? It would also be a good idea to stop passing so many things by pointer."
#warning "We can't be naive about it though, in some situations (such as adding one vector to another) it's better to use the pointer functions rather than doing an extra copy."

#warning "Should resource arrays be sorted? Then we could use a binary search to find particular items by name."
#warning "What if we just load everything in alphabetical order? Is that possible?"

#warning "When we implement intrinsics, use '_MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON)' to better handle subnormal numbers."
#warning "People suggest that '-ffast-math' and '-Ox' should do this anyway, but this doesn't seem to be the case on my system."
/* https://stackoverflow.com/questions/9314534/why-does-changing-0-1f-to-0-slow-down-performance-by-10x */

#warning "Our modules define a lot of functions that are identical or almost identical."
#warning "Would it be possible to avoid defining the duplicate functions by using macros?"
#warning "We could even go so far as to define the entire set of functions using a single macro."

#warning "Can we combine some of our memory allocations (for components of models, texture groups, skeletons, etcetera) into just one?"
#warning "This probably won't be as necessary when we create binary formats for things."
#warning "Besides being harder to allocate, this is also somewhat harmful if we need to reallocate individual arrays."

#warning "We need to implement mipmaps for textures and levels of detail for models."
#warning "Texture groups should store scrolling information (maybe other things too?) per-animation."

#warning "At the moment, particle systems have only a single renderer (more or less equivalent to Source's 'animated sprite' renderer."
#warning "It would be good to have a 'line' mode for rendering particles so we can render trails:"
/* https://www.reddit.com/r/gamedev/comments/387pwc/rendering_3d_trails/ */
/* https://www.eveonline.com/news/view/re-inventing-the-trails */
#warning "Maybe use the Source engine for inspiration, as their rope renderer is similar."
#warning "The particles should really just store points and directions."
#warning "We should interpolate between the points using a spline curve, then somehow generating a triangle strip for them."
#warning "Check the polyboard technique in 'Mathematics for 3D Game Programming and Computer Graphics -- it's exactly what we want."
#warning "It would also be a good idea to add something like Source's 'control points':"
/* https://developer.valvesoftware.com/wiki/Control_Point_(particles) */

#warning "We should try and put our graphics rendering code all in one place."
#warning "The shader-related code could use some simplification."
#warning "Maybe use a uniform buffer object for the view-projection matrix, too."
#warning "It would be good to have a proper render queue that sorts objects based on their textures."
#warning "Essentially, what we want is something like the render loop in 'Approaching Zero Driver Overhead'."
#warning "Another idea is to use the method presented in 'Order your graphics draw calls around!'"
/* https://realtimecollisiondetection.net/blog/?p=86 */
/* https://computergraphics.stackexchange.com/questions/37/what-is-the-cost-of-changing-state */
/* https://gamedev.net/tutorials/programming/graphics/opengl-api-overhead-r4614/ */

#warning "Maybe limit the scope of global variables that don't need to be seen from the outside?"
#warning "Our for loops should probably be using '!=' rather than '<' when iterating over pointers."

#warning "The free functions for most of the memory allocators kind of suck."
#warning "We should probably have some way of coalescing consecutive free blocks."
#warning "This would help improve coherence when allocating new arrays of elements."
#warning "It might be good to delete the unused 'FreeArray' functions, too."

#warning "We should eventually update the naming conventions for our math functions."
#warning "I sort of like what Dan does. Instead of using the 'Out' suffix, just use 'P'!"
#warning "Might as well clean up some of the useless 'SubtractFrom' functions, too."

#warning "We use lookup tables in a lot of places where it would probably be more readable to just use a switch statement."


/**
*** Important To-Do List:
***
***	1.	Finish the new particle system.
***
***	2.	Implement the remaining physics colliders (point, sphere, capsule
***		and AABB). It might be smart to have a file for each collision check.
***
***	3.	Finish the last few physics constraints (hinge, revolute, prismatic).
***		Don't worry about the extra features such as restitution, softness,
***		warm starting or Gauss-Seidel just yet.
***
***	4.	Add motors for the physics constraints.
***
***	5.	Renderer overhaul (see the warnings above).
**/


/**
*** Lighting Stuff:
***
*** 1.	We should eventually implement a clustered forward renderer to allow
*** 	for lots of light sources. It might be good to do a depth prepass,
*** 	but this sounds slow, especially if we have skeletal meshes.
*** 	https://www.aortiz.me/2018/12/21/CG.html
*** 	Here's another implementation that skips the depth prepass:
*** 	https://flexw.github.io/posts/rendering-thousands-of-point-lights-efficiently/
***
***	2.	For static lighting, we can use shadow maps and shadow volumes for
*** 	direct lighting and light probe volumes for indirect lighting.
***
*** 3.	I need to think more about dynamic lighting.
**/


#if 0
memoryPool g_shaderManager;
memoryPool g_sceneManager;


typedef uint_least8_t renderTargetType;
// Describes how to render a scene.
typedef struct renderTarget {
    camera cam;

    // Common graphics options.
    // Anti-aliasing?
    // Filtering mode?
    // Stuff like this.

    // We can render either to a
    // texture or part of the screen.
    union {
        rectangle viewport;
        texture tex;
    } target;
    renderTargetType type;
} renderTarget;

/*
** Contains the information required to render
** a scene from a particular point of view.
**
** Almost all of the data here is able
** to be manipulated from within a scene.
*/
typedef struct renderer {
    // Array of shaders that the scenes
    // we're rendering can choose from.
    shader **shaders;
    scene *scn;
    renderTarget target;
} renderer;
#endif


#include "vec3.h"
int main(int argc, char **argv){
	/*const vec3 camera = {.x = 0.f, .y = 1.f, .z = 5.f};
	const float r = 0.5f;
	const vec3 polyline[9] = {
		{.x = 0.000000f, .y = 0.000000f, .z =  0.000000f},
		{.x = 0.581990f, .y = 0.166556f, .z = -0.773722f},
		{.x = 1.176875f, .y = 0.342076f, .z = -1.830791f},
		{.x = 1.460396f, .y = 0.486510f, .z = -2.955424f},
		{.x = 1.621058f, .y = 0.568548f, .z = -4.382479f},
		{.x = 1.536001f, .y = 0.577663f, .z = -5.781182f},
		{.x = 1.195776f, .y = 0.504741f, .z = -7.236589f},
		{.x = 0.657087f, .y = 0.281417f, .z = -8.389574f},
		{.x = 0.000000f, .y = 0.000000f, .z = -9.173982f}
	};
	vec3 polyboard[18];
	unsigned int i;
	for(i = 0; i < 9; ++i){
		const vec3 z = vec3NormalizeVec3C(vec3SubtractVec3C(camera, polyline[0]));
		vec3 t;
		vec3 rtz;
		if(i == 0){
			t = vec3NormalizeVec3C(vec3SubtractVec3C(polyline[1], polyline[0]));
		}else if(i == 8){
			t = vec3NormalizeVec3C(vec3SubtractVec3C(polyline[8], polyline[7]));
		}else{
			t = vec3NormalizeVec3C(vec3SubtractVec3C(polyline[i+1], polyline[i-1]));
		}
		rtz = vec3MultiplySC(vec3NormalizeVec3C(vec3CrossVec3C(t, z)), r);
		polyboard[2*i] = vec3AddVec3C(polyline[i], rtz);
		polyboard[2*i+1] = vec3SubtractVec3C(polyline[i], rtz);
		printf("v %f %f %f\nl %u %u\nv %f %f %f\nl %u %u\n",
			polyboard[2*i].x, polyboard[2*i].y, polyboard[2*i].z, i+1, 9+2*i+1,
			polyboard[2*i+1].x, polyboard[2*i+1].y, polyboard[2*i+1].z, i+1, 9+2*i+2
		);
	}

	return(0);*/
	program prg;

	if(programInit(&prg, argv[0])){
		programLoop(&prg);
	}
	programClose(&prg);

	return(0);
}