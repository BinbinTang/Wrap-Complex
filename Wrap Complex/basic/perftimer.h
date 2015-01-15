#ifndef __RGI_PERFTIMER_H__
#define __RGI_PERFTIMER_H__

#if defined(unix)
#include <sys/time.h>
#endif

/////////////////////////////////////////////////////////////////////////////
//
// class PerfTimer
//
//
class PerfTimer {
public:

	//
	// Life cycle methods.
	//

	PerfTimer(const int startTimer = 0);
		// Constructor.

	~PerfTimer();
		// Destructor.

	//
	// Enable methods.
	//

	inline int enabled(void) const;

	// 
	// Timer methods.
	//

	void start(void);
		// Resets and starts the timer.

	void stop(void);
		// Stops the timer.

	void reset(void);
		// Sets the timer time back to zero.

	double time(void);
		// Returns the current time of the timer.

private:

	int enabled_;
	int running_;

#ifdef WIN32
	void *startTime_;
	void *stopTime_;

	double secPerCount_;
	double secPer32Count_;

#elif defined(unix)
	struct timeval startTime_;
	struct timeval stopTime_;
#endif
};

//
// Bodies for PerfTimer inline methods.
//

inline int PerfTimer::
enabled(void) const
{
	return enabled_;
}

#endif
