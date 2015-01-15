#ifndef __RGI_LOGFILE_H__
#define __RGI_LOGFILE_H__

//class ofstream;
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdio>

using namespace std;

class LogFile
{
public:
	LogFile (const char *filename);
	~LogFile ();

	void enable ();
	void disable ();
	int isEnabled () const;

	void flush ();

	LogFile &operator << (const char *);
	LogFile &operator << (int);
	LogFile &operator << (double);

	LogFile &operator << (void (*) (LogFile *));

protected:
	void createStream ();

private:
	int enabled_;
	const char *fileName_;
	ofstream *os_;
};

void endl (LogFile *);

extern LogFile theLog;

#endif
