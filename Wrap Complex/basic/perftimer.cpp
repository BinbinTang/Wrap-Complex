#ifndef __PERFTIMER_H__
#   include "perftimer.h"
#endif

#include "basic.h"

#ifdef WIN32

#include <stdio.h>

#define _X86_

#include <windef.h>
#include <winbase.h>

typedef LARGE_INTEGER *LARGE_INTEGER_P;

#elif defined(unix)

#else

#error unknown system

#endif

#include <math.h>

PerfTimer::
PerfTimer(const int startTimer)
{
#ifdef WIN32

	startTime_ = new LARGE_INTEGER;
	stopTime_ = new LARGE_INTEGER;

	LARGE_INTEGER countsPerSec;

	if (QueryPerformanceFrequency(&countsPerSec)) {

		secPerCount_ = 1.0/(countsPerSec.LowPart + 
						    pow(2.0, 32.0)*countsPerSec.HighPart);
		secPer32Count_ = pow(2.0, 32.0)*secPerCount_;

		enabled_ = TRUE;

		if (startTimer) {

			start();
		}
	}
	else {

		enabled_ = FALSE;
	}

#elif defined(unix)
	start();
	enabled_ = TRUE;
#endif
}

PerfTimer::
~PerfTimer()
{

#ifdef WIN32

	delete LARGE_INTEGER_P(startTime_);
	delete LARGE_INTEGER_P(stopTime_);

	startTime_ = NULL;
	stopTime_ = NULL;
#endif 
}

void PerfTimer::
start(void)
{
	running_ = TRUE;

#ifdef WIN32

	QueryPerformanceCounter(LARGE_INTEGER_P(startTime_));

#elif defined(unix)
	gettimeofday(&startTime_, 0);
#endif
}

void PerfTimer::
stop(void)
{
#ifdef WIN32

	QueryPerformanceCounter(LARGE_INTEGER_P(stopTime_));

#elif defined(unix)
	gettimeofday(&stopTime_, 0);
#endif

	running_ = FALSE;
}

void PerfTimer::
reset(void)
{
#ifdef WIN32

	QueryPerformanceCounter(LARGE_INTEGER_P(startTime_));
	LARGE_INTEGER_P(stopTime_)->HighPart = LARGE_INTEGER_P(startTime_)->HighPart;
	LARGE_INTEGER_P(stopTime_)->LowPart = LARGE_INTEGER_P(startTime_)->LowPart;

#elif defined(unix)
	gettimeofday(&startTime_, 0);
	stopTime_.tv_sec = startTime_.tv_sec;
	stopTime_.tv_usec = startTime_.tv_usec;
#endif

	running_ = FALSE;
}

double PerfTimer::
time(void)
{
	double timeRet = 0.0;

#ifdef WIN32

	LARGE_INTEGER curTime;

	if (running_) {

		QueryPerformanceCounter(&curTime);
	}
	else {

		curTime.LowPart = LARGE_INTEGER_P(stopTime_)->LowPart;
		curTime.HighPart = LARGE_INTEGER_P(stopTime_)->HighPart;
	}

	LARGE_INTEGER deltaTime;

	deltaTime.LowPart = curTime.LowPart - LARGE_INTEGER_P(startTime_)->LowPart;
	deltaTime.HighPart = curTime.HighPart - LARGE_INTEGER_P(startTime_)->HighPart;

	if (curTime.LowPart < LARGE_INTEGER_P(startTime_)->LowPart) {

		deltaTime.HighPart = deltaTime.HighPart - 1;
	}

	timeRet = deltaTime.HighPart*secPer32Count_
		+ deltaTime.LowPart*secPerCount_;

#elif defined(unix)
	struct timeval curTime;

	if (running_) {
		gettimeofday(&curTime, 0);
	} else {
		curTime.tv_sec = stopTime_.tv_sec;
		curTime.tv_usec = stopTime_.tv_usec;
	}

	const double curTimeSec = curTime.tv_sec + 1e-6*curTime.tv_usec;
	const double startTimeSec = startTime_.tv_sec + 1e-6*startTime_.tv_usec;

	timeRet = curTimeSec - startTimeSec;
#endif

	return timeRet;
}
