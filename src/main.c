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


int main(int argc, char **argv){
	program prg;

	if(programInit(&prg, argv[0])){
		programLoop(&prg);
	}
	programClose(&prg);

	return(0);
}