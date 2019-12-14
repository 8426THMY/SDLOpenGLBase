#ifndef utilTypes_h
#define utilTypes_h


#include <stdint.h>


typedef uint_least8_t byte_t;
typedef int return_t;
typedef uint_least8_t flags_t;


#define invalidValue(x)   ((typeof(x))-1)
#define valueIsInvalid(x) (x == ((typeof(x))-1))

#define flagsCheck(flags, bits)    (flags & (bits))
#define flagsAreSet(flags, bits)   (flags & (bits)) != 0
#define flagsAreUnset(flags, bits) (flags & (bits)) == 0


#endif