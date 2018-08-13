#include "vector.h"


#include <string.h>


unsigned char vectorInit(vector *v, const size_t elementSize){
	v->data = malloc(elementSize);
	if(v->data == NULL){
		return(0);
	}
	v->elementSize = elementSize;
	v->capacity = 1;
	v->size = 0;

	return(1);
}


unsigned char vectorResize(vector *v, const size_t capacity){
	void *tempData = realloc(v->data, v->elementSize * capacity);
	if(tempData != NULL){
		v->data = tempData;
		v->capacity = capacity;

		return(1);
	}

	return(0);
}

unsigned char vectorAdd(vector *v, const void *data, const size_t num){
	if(num > 0){
		v->size += num;
		if(v->size >= v->capacity){
			if(!vectorResize(v, v->size * 2)){
				return(0);
			}
		}
		memcpy(v->data + (v->size - num) * v->elementSize, data, v->elementSize * num);

		return(1);
	}

	return(0);
}

void vectorRemove(vector *v, const size_t pos){
	if(pos < v->size){
		--v->size;
		if(pos != v->size){
			memmove(v->data + pos * v->elementSize, v->data + (pos + 1) * v->elementSize, (v->size - pos) * v->elementSize);
		}
	}
}

void *vectorGet(const vector *v, const size_t pos){
	if(pos < v->size){
		return(v->data + pos * v->elementSize);
	}

	return(NULL);
}

void vectorSet(vector *v, const size_t pos, const void *data, const size_t num){
	if(pos + num <= v->size){
		memcpy(v->data + pos * v->elementSize, data, v->elementSize * num);
	}
}


void vectorClear(vector *v){
	free(v->data);
}