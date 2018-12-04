#ifndef memorySettings_h
#define memorySettings_h


#define MEMORY_KIBIBYTE 1024
#define MEMORY_MEBIBYTE (1024 * MEMORY_KIBIBYTE)

//We use a default heap size of 512 mebibytes.
#define MEMORY_HEAPSIZE (512 * MEMORY_MEBIBYTE)

#define MEMORY_USE_GLOBAL_MANAGER
#define MEMORY_USE_MODULE_MANAGER


#error "Modules should allocate a new object, then call the loading function on that object."


#endif