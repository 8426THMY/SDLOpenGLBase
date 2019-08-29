#include "state.h"


#include <stdlib.h>

#include "settingsProgram.h"


void stateObjInit(stateObject *stateObj, const size_t stateSize){
	stateObj->states = malloc(sizeof(*stateObj->states) * NUM_LOOKBACK_STATES);
	stateObj->numStates = 1;
	stateObj->states[0] = malloc(stateSize);
}


void stateObjShift(stateObject *stateObj, const size_t stateSize, void (*copyFunc)(const void *s1, void *s2), void (*deleteFunc)(void *s)){
	if(NUM_LOOKBACK_STATES > 1){
		// If the state array is full, we can use the memory we allocated for
		// the oldest state so we don't have to allocate more for the new one.
		if(stateObj->numStates >= NUM_LOOKBACK_STATES){
			// "tempState" stores a pointer to the oldest state,
			// which we reuse for the new one that we're creating.
			void *tempState = stateObj->states[NUM_LOOKBACK_STATES - 1];
			// If the object still exists, we need to copy the data from the latest state into our new one.
			if(stateObj->states[0] != NULL){
				if(copyFunc != NULL){
					copyFunc(stateObj->states[0], tempState);
				}else{
					memcpy(tempState, stateObj->states[0], stateSize);
				}

			// Otherwise, we'll need to delete the oldest state and store NULL in the new spot.
			}else{
				if(deleteFunc != NULL){
					deleteFunc(tempState);
				}
				free(tempState);
				tempState = NULL;
			}
			// Shift all the states over and set the newest one to the temporary state we made!
			memmove(&stateObj->states[1], &stateObj->states[0], sizeof(*stateObj->states) * (NUM_LOOKBACK_STATES - 1));
			stateObj->states[0] = tempState;

		// If we haven't filled up the state array, we'll need to allocate memory for the new state.
		}else{
			memmove(&stateObj->states[1], &stateObj->states[0], sizeof(*stateObj->states) * stateObj->numStates);
			// Because stateObj->states[0] will retain its old value after a memmove,
			// we only need to allocate memory if the object hasn't been removed.
			// If the object has been removed, stateObj->states[0] will be left as NULL.
			if(stateObj->states[0] != NULL){
				stateObj->states[0] = malloc(stateSize);
				if(copyFunc != NULL){
					copyFunc(stateObj->states[1], stateObj->states[0]);
				}else{
					memcpy(stateObj->states[0], stateObj->states[1], stateSize);
				}
			}
			++stateObj->numStates;
		}
	}
}

// Remove our oldest states, but keep the earlier ones.
void stateObjRemove(stateObject *stateObj, void (*deleteFunc)(void *s)){
	// Shift all the states we've stored over to the right, then set the latest one to NULL.
	memmove(&stateObj->states[1], &stateObj->states[0], sizeof(*stateObj->states) * (NUM_LOOKBACK_STATES - 1));
	stateObj->states[0] = NULL;
}


// Delete our states, starting from the oldest, until we reach a NULL pointer!
void stateObjDelete(stateObject *stateObj, void (*deleteFunc)(void *s)){
	while(stateObj->numStates > 0){
		--stateObj->numStates;

		void *currentState = stateObj->states[stateObj->numStates];
		if(currentState != NULL){
			if(deleteFunc != NULL){
				deleteFunc(currentState);
			}
			free(currentState);

		// If the currentState is NULL, it means this is when the object was removed from the world.
		}else{
			break;
		}
	}

	free(stateObj->states);
}