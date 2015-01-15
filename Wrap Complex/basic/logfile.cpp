#include <basic/logfile.h>
//#include <fstream>

#if defined (_MSC_VER)
LogFile theLog ("c:\\rgidebug.txt");
#elif defined (UNIX)
LogFile theLog ("/tmp/rgidebug.txt");
#else
#error Dont know where to put the log file
#endif

LogFile::
LogFile (const char *name)
{
	fileName_ = name;
	os_ = 0;
}

LogFile::
~LogFile ()
{
	if (os_)
	{
		os_->close ();
		delete os_;
	}
}

void LogFile::
enable ()
{
	enabled_ = 1;
}

void LogFile::
disable ()
{
	if (os_)
		os_->flush ();
	enabled_ = 0;
}

int LogFile::
isEnabled () const
{
	return enabled_;
}

LogFile &LogFile::
operator << (const char *str)
{
	if (!enabled_)
		return *this;

	if (!os_)
		createStream ();

	*os_ << str;
	return *this;
}

LogFile &LogFile::
operator << (int n)
{
	if (!enabled_)
		return *this;

	if (!os_)
		createStream ();

	*os_ << n;
	return *this;
}

LogFile &LogFile::
operator << (double x)
{
	if (!enabled_)
		return *this;

	if (!os_)
		createStream ();

	*os_ << x;
	return *this;
}

LogFile &LogFile::
operator << (void (* function) (LogFile *))
{
	(*function) (this);
	return *this;
}

void LogFile::
createStream ()
{
	os_ = new ofstream (fileName_);
}

void LogFile::
flush ()
{
	if (!enabled_)
		return;

	if (!os_)
		createStream ();

	os_->flush ();
}

void endl (LogFile *logFile)
{
	*logFile << "\n";
	logFile->flush ();
}
