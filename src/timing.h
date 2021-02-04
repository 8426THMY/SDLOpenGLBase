#ifndef timing_h
#define timing_h


#include <stdint.h>


#ifdef _WIN32
	#include <windows.h>

	typedef LARGE_INTEGER timerVal_t;
#else
	#if HAVE_CLOCK_GETTIME
		#include <time.h>

		typedef struct timespec timerVal_t;
	#else
		typedef struct timeval timerVal_t;
	#endif
#endif

typedef uint32_t time32_t;
typedef uint64_t time64_t;


void initTiming();

time32_t elapsedTime(const timerVal_t start, const timerVal_t end);
float elapsedTimeFloat(const timerVal_t start, const timerVal_t end);

time32_t getTime();
float getTimeFloat();

timerVal_t startTimer();
time32_t endTimer(const timerVal_t start);
float endTimerFloat(const timerVal_t start);

void sleepResolution(const time32_t ms, const unsigned int res);
void sleepInaccurate(const time32_t ms);
void sleepAccurate(const time32_t ms);
void sleepBusy(const time32_t ms);


#endif