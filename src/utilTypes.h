#ifndef utilTypes_h
#define utilTypes_h


#include <stdint.h>


typedef uint_least8_t byte_t;
typedef uint_least8_t flags_t;

typedef int return_t;


#define valueInvalid(type) ((type)-1)
#define valueIsInvalid(x, type) ((x) == ((type)-1))

#define flagsSet(flags, bits) ((flags) |= (bits))
#define flagsUnset(flags, bits) ((flags) &= ~(bits))
#define flagsMaskSet(flags, bits) ((flags) &= (bits))

#define flagsMask(flags, bits)           ((flags) & (bits))
// True if the flags contains a subset of the bits.
#define flagsContainsSubset(flags, bits) (((flags) & (bits)) != 0)
// True if the flags contain the entire set of bits.
#define flagsContainsSet(flags, bits)    (((flags) & (bits)) == (bits))




#endif