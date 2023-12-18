#ifndef timer_h
#define timer_h


#include <stdint.h>


#ifdef _WIN32
	#include <windows.h>

	typedef LARGE_INTEGER timerVal;
#else
	#include <sys/time.h>

	#if HAVE_CLOCK_GETTIME
		typedef struct timespec timerVal;
	#else
		typedef struct timeval timerVal;
	#endif
#endif

typedef uint32_t time32;
typedef uint64_t time64;


void timerInit();

time32 timerElapsedTime(const timerVal start, const timerVal end);
float timerElapsedTimeFloat(const timerVal start, const timerVal end);

time32 timerGetTime();
float timerGetTimeFloat();

timerVal timerStart();
time32 timerStop(const timerVal start);
float timerStopFloat(const timerVal start);

void sleepResolution(const time32 ms, const unsigned int res);
void sleepInaccurate(const time32 ms);
void sleepAccurate(const time32 ms);
void sleepBusy(const time32 ms);
void sleepUntil(const time32 end);
void sleepUntilFloat(const float end);


#endif