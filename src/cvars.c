#include "cvars.h"


#include <stdlib.h>
#include <string.h>

#include "command.h"


return_t cv_prg_running = 1;

int cv_mouse_dx;
int cv_mouse_dy;


// Forward-declare any helper functions!
#ifdef C_MOUSEMOVE_FAST
static int mouseMotionStrToInt(const char *str);
#endif


void c_exit(commandSystem *const restrict cmdSys, const size_t argc, const char **const restrict argv){
	cv_prg_running = 0;
}

void c_mousemove(commandSystem *const restrict cmdSys, const size_t argc, const char **const restrict argv){
	if(argc == 2){
		#ifdef C_MOUSEMOVE_FAST
		// If the first character is greater than the command limit,
		// it must be a flag byte and not a numerical value.
		if(argv[0][0] >= COMMAND_SPECIAL_CHAR_LIMIT){
			cv_mouse_dx += mouseMotionStrToInt(argv[0]);
			cv_mouse_dy += mouseMotionStrToInt(argv[1]);

		// Otherwise, handle the arguments normally.
		}else{
		#endif
			cv_mouse_dx += strtol(argv[0], NULL, 10);
			cv_mouse_dy += strtol(argv[1], NULL, 10);
		#ifdef C_MOUSEMOVE_FAST
		}
		#endif
	}
}


#ifdef C_MOUSEMOVE_FAST
/*
** Convert a signed integer back from the special
** string format used for the command system.
*/
static int mouseMotionStrToInt(const char *str){
	unsigned int curIndex = 0;
	const char flagChar = *str - COMMAND_SPECIAL_CHAR_LIMIT;
	int i;

	memcpy(&i, &str[1], sizeof(i));
	// Reconstruct each byte of the original integer.
	for(; curIndex < sizeof(i); ++curIndex, ++str){
		// If the flag for this byte is set, add 60*2^(8(n-1))
		// to it, where n is the index of the modified byte.
		if(flagChar & 1 << curIndex){
			i -= COMMAND_SPECIAL_CHAR_LIMIT << (curIndex << 3);
		}
	}

	return(i);
}
#endif