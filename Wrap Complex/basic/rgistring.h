#ifndef __RGI_RGISTRING_H__
#define __RGI_RGISTRING_H__

#include <iostream>

#if !defined(USE_QSTRING)
#if !defined(USE_ASCII)
#if !defined(USE_STRING)
#if !defined(USE_WSTRING)
#define USE_QSTRING 1
#endif
#endif
#endif
#endif

#if !defined(USE_QSTRING)
#	include <basic/basic.h>
#endif

#if defined(USE_QSTRING) && defined(UNIX)
#	include <basic/unix_basic.h>
#endif

#if defined(USE_STRING) || defined(USE_WSTRING)
#	include <string>
#endif

class QString;
class RgiStringAdaptor;
class RgiTranslator;
class RgiFormat;
class RgiCString;

class RgiString {
public:
	RgiString ();
#if defined(USE_ASCII)
	RgiString (const char *);
#elif defined (USE_QSTRING)
	RgiString (const QString &);
#elif defined (USE_STRING)
	RgiString (const string &);
#elif defined (USE_WSTRING)
	RgiString (const wstring &);
#endif

	RgiString (const RgiString &);
	~RgiString();

	static RgiString null;
	static RgiString space;

	RgiString &operator = (const RgiString &);
	int operator == (const RgiString &) const;
	int operator != (const RgiString &) const;

	int operator[] (int idx) const;

	int length () const;
	void empty ();
	int isNull () const;
	int isEmpty () const;

	RgiString lower () const;
	RgiString upper () const;
	RgiString &append (const RgiString &);

	int contains(const RgiString &str, int cs=1) const;
		// Returns the number of overlapping occurences of str.
		// cs is case sensitivity.

	RgiString replace (int start, int end, const RgiString &) const;

#if defined(USE_ASCII)
	RgiString &operator = (const char *);
	operator const char *() const;
#elif defined (USE_QSTRING)
	RgiString &operator = (const QString &);
	operator const QString &() const;
#elif defined (USE_STRING)
	RgiString &operator = (const string &);
	operator const string &() const;
#elif defined (USE_WSTRING)
	RgiString &operator = (const wstring &);
	operator const wstring &() const;
#endif

	int toInt (int *ok=0) const;
	double toDouble (int *ok=0) const;

	void setFrom (const RgiStringAdaptor &);

	const char *latin1 () const;
		// The char * returned here is only valid until the next 
		// call to latin1() or the RgiString object is deleted.  
		// The RgiString object takes care of the memory cleanup.

	friend class RgiStringAdaptor;
	friend class RgiTranslator;

protected:
	RgiString (int x);
	RgiString (unsigned long x);
	RgiString (double x);
	RgiString (int x, const RgiFormat &format);
	RgiString (unsigned long x, const RgiFormat &format);
	RgiString (double x, const RgiFormat &format);

#if !defined (USE_ASCII)
	RgiString (const char *);
#endif

private:
#if defined(USE_ASCII)
	char *string_;
#elif defined(USE_QSTRING)
	QString *string_;
#elif defined (USE_STRING)
	string string_;
#elif defined (USE_WSTRING)
	string wstring_;
#endif

	void findLatin1Rep(void);
	char *latin1Rep_;
};

class RgiFormat {
public:
	RgiFormat (const char *fmt);
	~RgiFormat ();

	friend class RgiString;

private:
	char *fmt_;
};

class RgiStringAdaptor {
public:
	RgiStringAdaptor () : string_() {}
	RgiStringAdaptor (int x) : string_(x) {}
	RgiStringAdaptor (double x) : string_(x) {}
	RgiStringAdaptor (unsigned long x) : string_(x) {}
	RgiStringAdaptor (int x, const RgiFormat &f) : string_(x, f) {}
	RgiStringAdaptor (double x, const RgiFormat &f) : string_(x, f) {}
	RgiStringAdaptor (unsigned long x, const RgiFormat &f) : string_(x, f) {}
	RgiStringAdaptor (const char *x) : string_(x) {}
	RgiStringAdaptor (const RgiCString &x);
	RgiStringAdaptor (const RgiString &x) : string_(x) {}

	const RgiString &string () const { return string_; }

private:
	RgiString string_;
};

ostream &operator << (ostream &, const RgiString &);

#endif
