#ifndef timer_h
#define timer_h


#include <stdint.h>


#ifdef _WIN32
	#include <windows.h>

	typedef LARGE_INTEGER timerVal_t;
#else
	#include <sys/time.h>

	#if HAVE_CLOCK_GETTIME
		typedef struct timespec timerVal_t;
	#else
		typedef struct timeval timerVal_t;
	#endif
#endif

typedef uint32_t time32_t;
typedef uint64_t time64_t;


void timerInit();

time32_t timerElapsedTime(const timerVal_t start, const timerVal_t end);
float timerElapsedTimeFloat(const timerVal_t start, const timerVal_t end);

time32_t timerGetTime();
float timerGetTimeFloat();

timerVal_t timerStart();
time32_t timerStop(const timerVal_t start);
float timerStopFloat(const timerVal_t start);

void sleepResolution(const time32_t ms, const unsigned int res);
void sleepInaccurate(const time32_t ms);
void sleepAccurate(const time32_t ms);
void sleepBusy(const time32_t ms);
void sleepUntil(const time32_t end);
void sleepUntilFloat(const float end);


#endif