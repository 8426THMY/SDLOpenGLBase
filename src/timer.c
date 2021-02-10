#include "timer.h"


#ifdef _WIN32
	static time32_t freq;
	static float rfreq;
#else
	#include <stddef.h>
	#include <errno.h>

	#if !HAVE_NANOSLEEP
		#include <sys/select.h>
	#endif

	#if HAVE_CLOCK_GETTIME
		#ifdef CLOCK_MONOTONIC_RAW
			#define TIMING_MONOTONIC_CLOCK CLOCK_MONOTONIC_RAW
		#else
			#define TIMING_MONOTONIC_CLOCK CLOCK_MONOTONIC
		#endif
	#endif

	// Forward-declare any helper functions!
	#if HAVE_NANOSLEEP
	static void sleepUnix(const time32_t ms);
	#else
	static void sleepUnix(time32_t ms);
	#endif
#endif


// Stores the time value at which the timing system was initialized.
static timerVal_t first;


void timerInit(){
	#ifdef _WIN32
		LARGE_INTEGER li;
		// This should only fail on versions of Windows before XP.
		if(!QueryPerformanceFrequency(&li)){
			freq = 1;
			rfreq = 1.f;
		}else{
			freq = li.u.LowPart / 1000;
			rfreq = 1000.f / (float)li.QuadPart;
		}
		QueryPerformanceCounter(&first);
	#else
		#if HAVE_CLOCK_GETTIME
		clock_gettime(TIMING_MONOTONIC_CLOCK, &first);
		#else
		gettimeofday(&first, NULL);
		#endif
	#endif
}


/*
** Return how many milliseconds have
** passed between "start" and "end".
*/
time32_t timerElapsedTime(const timerVal_t start, const timerVal_t end){
	#ifdef _WIN32
		return((end.u.LowPart - start.u.LowPart) / freq);
	#else
		#if HAVE_CLOCK_GETTIME
		return((end.tv_sec - start.tv_sec) * 1000 + (end.tv_nsec - start.tv_nsec) / 1000000);
		#else
		return((end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000);
		#endif
	#endif
}

/*
** Return how many milliseconds have
** passed between "start" and "end".
*/
float timerElapsedTimeFloat(const timerVal_t start, const timerVal_t end){
	#ifdef _WIN32
		return((float)(end.u.LowPart - start.u.LowPart) * rfreq);
	#else
		#if HAVE_CLOCK_GETTIME
		return((float)((end.tv_sec - start.tv_sec) * 1000) + (float)(end.tv_nsec - start.tv_nsec) / 1000000.f);
		#else
		return((float)((end.tv_sec - start.tv_sec) * 1000) + (float)(end.tv_usec - start.tv_usec) / 1000.f);
		#endif
	#endif
}


/*
** Return how many milliseconds have elapsed
** since the timing system was initialized.
*/
time32_t timerGetTime(){
	#ifdef _WIN32
		timerVal_t now;
		QueryPerformanceCounter(&now);
	#else
		#if HAVE_CLOCK_GETTIME
		timerVal_t now;
		clock_gettime(TIMING_MONOTONIC_CLOCK, &now);
		#else
		timerVal_t now;
		gettimeofday(&now, NULL);
		#endif
	#endif

	return(timerElapsedTime(first, now));
}

/*
** Return how many milliseconds have elapsed
** since the timing system was initialized.
*/
float timerGetTimeFloat(){
	#ifdef _WIN32
		timerVal_t now;
		QueryPerformanceCounter(&now);
	#else
		#if HAVE_CLOCK_GETTIME
		timerVal_t now;
		clock_gettime(TIMING_MONOTONIC_CLOCK, &now);
		#else
		timerVal_t now;
		gettimeofday(&now, NULL);
		#endif
	#endif

	return(timerElapsedTimeFloat(first, now));
}


/*
** Returns the current tick, which is mostly
** useless for timing without the frequency.
*/
timerVal_t timerStart(){
	#ifdef _WIN32
		timerVal_t start;
		QueryPerformanceCounter(&start);
	#else
		#if HAVE_CLOCK_GETTIME
		timerVal_t start;
		clock_gettime(TIMING_MONOTONIC_CLOCK, &start);
		#else
		timerVal_t start;
		gettimeofday(&start, NULL);
		#endif
	#endif

	return(start);
}

// Return how many milliseconds have elapsed since "start".
time32_t timerStop(const timerVal_t start){
	#ifdef _WIN32
		timerVal_t end;
		QueryPerformanceCounter(&end);
	#else
		#if HAVE_CLOCK_GETTIME
		timerVal_t end;
		clock_gettime(TIMING_MONOTONIC_CLOCK, &end);
		#else
		timerVal_t end;
		gettimeofday(&end, NULL);
		#endif
	#endif

	return(timerElapsedTime(start, end));
}

// Return how many milliseconds have elapsed since "start".
float timerStopFloat(const timerVal_t start){
	#ifdef _WIN32
		timerVal_t end;
		QueryPerformanceCounter(&end);
	#else
		#if HAVE_CLOCK_GETTIME
		timerVal_t end;
		clock_gettime(TIMING_MONOTONIC_CLOCK, &end);
		#else
		timerVal_t end;
		gettimeofday(&end, NULL);
		#endif
	#endif

	return(timerElapsedTimeFloat(start, end));
}


/*
** Sleep with a particular timer resolution (Windows only).
** The system's default resolution is used if "res" is '0'.
*/
void sleepResolution(const time32_t ms, const unsigned int res){
	#ifdef _WIN32
		timeBeginPeriod(res);
		Sleep(ms);
		timeEndPeriod(res);
	#else
		sleepUnix(ms);
	#endif
}

/*
** Although possibly more lenient on the processor,
** it tends to sleep for much longer than specified.
*/
void sleepInaccurate(const time32_t ms){
	#ifdef _WIN32
		timeBeginPeriod(ms);
		Sleep(ms);
		timeEndPeriod(ms);
	#else
		sleepUnix(ms);
	#endif
}

// Good accuracy and decently low power usage.
void sleepAccurate(const time32_t ms){
	#ifdef _WIN32
		timeBeginPeriod(1);
		Sleep(ms);
		timeEndPeriod(1);
	#else
		sleepUnix(ms);
	#endif
}

// Extremely accurate but not very kind on the processor.
void sleepBusy(const time32_t ms){
	timerVal_t now = timerStart();
	#ifdef _WIN32
		const time64_t end = now.QuadPart + ms * freq;
		while(now = timerStart(), now.QuadPart < end);
	#else
		#if HAVE_CLOCK_GETTIME
		const time64_t end = ((time64_t)now.tv_sec * 1000000000 + (time64_t)now.tv_nsec) + (time64_t)ms * 1000000;
		while(now = timerStart(), ((time64_t)now.tv_sec * 1000000000 + (time64_t)now.tv_nsec) < end);
		#else
		const time64_t end = ((time64_t)now.tv_sec * 1000000000 + (time64_t)now.tv_usec) + (time64_t)ms * 1000000;
		while(now = timerStart(), ((time64_t)now.tv_sec * 1000000000 + (time64_t)now.tv_usec) < end);
		#endif
	#endif
}

void sleepUntil(const time32_t end){
	const time32_t timeLeft = end - timerGetTime();
	if(timeLeft >= 1){
		sleepAccurate(timeLeft);
	}
}

void sleepUntilFloat(const float end){
	const float timeLeft = end - timerGetTimeFloat();
	if(timeLeft >= 1.f){
		sleepAccurate((time32_t)timeLeft);
	}
}


#ifndef _WIN32
// On UNIX derivatives, we only have a single sleep function (and busy-waiting).
#if HAVE_NANOSLEEP
static void sleepUnix(const time32_t ms){
#else
static void sleepUnix(time32_t ms){
#endif

	int r;

	#if HAVE_NANOSLEEP
	const time32_t s = ms / 1000;
	timerVal_t tv = {
		.tv_sec = s;
		.tv_nsec = (ms - s * 1000) * 1000000;
	};
	#else
	time32_t s = ms / 1000;
	struct timeval tv;
	gettimeofday(&tv, NULL);
	#endif

	// We may get interrupted while sleeping, so
	// keep trying until we've slept for long enough.
	do {
		#if HAVE_NANOSLEEP
		const struct timespec ts = {
			.tv_sec = tv.tv_sec;
			.tv_nsec = tv.tv_nsec;
		};

		errno = 0;
		r = nanosleep(&ts, &tv);
		#else
		struct timeval now;
		const time32_t elapsed = (gettimeofday(&now, NULL), timerElapsedTime(tv, now));
		if(elapsed >= ms){
			return;
		}
		ms -= elapsed;

		s = ms / 1000;
		tv.tv_sec = s;
		tv.tv_usec = (ms - s * 1000) * 1000;

		errno = 0;
		r = select(0, NULL, NULL, NULL, &tv);
		#endif
	} while(r == -1 && errno == EINTR);
}
#endif