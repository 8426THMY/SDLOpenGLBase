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
#warning "Alternatively, if this gets ugly, it would almost certainly be better to define these functions using macros."

#warning "Can we combine some of our memory allocations (for components of models, texture groups, skeletons, etcetera) into just one?"
#warning "This probably won't be as necessary when we create binary formats for things."

#warning "We need to implement mipmaps for textures and levels of detail for models."
#warning "Texture groups should store scrolling information (maybe other things too?) per-animation."

#warning "Maybe implement a 'line' mode for rendering particles rather than the current 'point' mode?"
#warning "This would be used for drawing trails, as in this post:"
/* https://www.reddit.com/r/gamedev/comments/387pwc/rendering_3d_trails/ */


#if 0
memoryPool g_shaderManager;
memoryPool g_sceneManager;


typedef uint_least8_t renderTargetType_t;
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
    renderTargetType_t type;
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


int main(int argc, char **argv){
	program prg;

	if(programInit(&prg, argv[0])){
		programLoop(&prg);
	}
	programClose(&prg);

	return(0);
}