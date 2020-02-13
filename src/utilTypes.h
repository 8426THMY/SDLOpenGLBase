#ifndef utilTypes_h
#define utilTypes_h


#include <stdint.h>


typedef uint_least8_t byte_t;
typedef uint_least8_t flags_t;

typedef int return_t;


#define invalidValue(x)   ((typeof(x))-1)
#define valueIsInvalid(x) ((x) == ((typeof(x))-1))

#define flagsSet(flags, bits) ((flags) |= (bits))
#define flagsUnset(flags, bits) ((flags) &= ~(bits))
#define flagsMask(flags, bits) ((flags) &= (bits))

#define flagsCheck(flags, bits)    ((flags) & (bits))
#define flagsAreSet(flags, bits)   (((flags) & (bits)) != 0)
#define flagsAreUnset(flags, bits) (((flags) & (bits)) == 0)


#endif