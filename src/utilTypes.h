#ifndef utilTypes_h
#define utilTypes_h


typedef unsigned char byte_t;
typedef int return_t;


#define INVALID_VALUE(x) ((typeof(x))-1)
#define VALUE_IS_INVALID(x) (x == ((typeof(x))-1))


#endif