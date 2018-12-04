#ifndef vector_h
#define vector_h


#include <stdlib.h>

#include "utilTypes.h"


typedef struct vector {
	void *data;
	size_t elementSize;
	size_t capacity;
	size_t size;
} vector;


return_t vectorInit(vector *v, const size_t elementSize);

return_t vectorResize(vector *v, const size_t capacity);
return_t vectorAdd(vector *v, const void *data, const size_t num);
void vectorRemove(vector *v, const size_t pos);
void *vectorGet(const vector *v, const size_t pos);
void vectorSet(vector *v, const size_t pos, const void *data, const size_t);

void vectorClear(vector *v);


#endif