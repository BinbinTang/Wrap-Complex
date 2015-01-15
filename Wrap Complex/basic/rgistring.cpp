#include <assert.h>
#include <cstdio>
#include <string>
#include <string.h>
#include <cstdlib>

#ifdef WIN32
#include <strstream>
#else
#include <strstream.h>
#endif

#include "rgistring.h"



#if defined (USE_QSTRING)
#   include <qstring.h>
#endif

#include <basic/basic.h>
#include "rgicstring.h"
//const char *RgiString::null=(const char *)0;

RgiString RgiString::null;
RgiString RgiString::space = RgiStringAdaptor(" ").string();

RgiString::
RgiString ()
{
#if defined (USE_ASCII)
	string_ = (char *)0;
#elif defined(USE_QSTRING)
	string_ = new QString();
#elif defined(USE_STRING)
	/* nothing */
#elif defined(USE_WSTRING)
	/* nothing */
#endif

	latin1Rep_ = (char *)(0);
}

#if defined(USE_QSTRING)
RgiString::
RgiString (const QString &string)
{
	string_ = new QString();
	*string_ = string;
	latin1Rep_ = (char *)(0);
}
#endif

RgiString::
RgiString (int x)
{
#if defined (USE_ASCII)
	ostrstream oss;
	oss << x << ends;
	string_ = oss.str ();
#elif defined(USE_QSTRING)
	char fbuf[20];
	sprintf(fbuf,"%%d");
	string_ = new QString();
	string_->sprintf (fbuf, x);
#elif defined(USE_STRING)
	ostrstream oss;
	oss << x << ends;
	string_ = oss.str ();
	delete[] oss.str ();
#endif
	latin1Rep_ = (char *)(0);
}

RgiString::
RgiString (unsigned long x)
{
#if defined (USE_ASCII)
	ostrstream oss;
	oss << x << ends;
	string_ = oss.str ();
#elif defined(USE_QSTRING)
	char fbuf[20];
	sprintf(fbuf,"%%d");
	string_ = new QString();
	string_->sprintf (fbuf, x);
#elif defined(USE_STRING)
	ostrstream oss;
	oss << x << ends;
	string_ = oss.str ();
	delete[] oss.str ();
#endif
	latin1Rep_ = (char *)(0);
}

RgiString::
RgiString (double x)
{
#if defined (USE_ASCII)
	ostrstream oss;
	oss << x << ends;
	string_ = oss.str ();
#elif defined(USE_QSTRING)
	char fbuf[20];
	sprintf(fbuf,"%%f");
	string_ = new QString();
	string_->sprintf (fbuf, x);
#elif defined(USE_STRING)
	ostrstream oss;
	oss << x << ends;
	string_ = oss.str ();
	delete[] oss.str ();
#endif
	latin1Rep_ = (char *)(0);
}

RgiString::
RgiString (int x, const RgiFormat &format)
{
#if defined (USE_ASCII)
	Assert(0);
	ostrstream oss;
	oss << x << ends;
	string_ = oss.str ();
#elif defined(USE_QSTRING)
	char fbuf[20];
	sprintf(fbuf,"%%%sd",format.fmt_);
	string_ = new QString();
	string_->sprintf (fbuf, x);
#elif defined(USE_STRING)
	Assert(0);
	ostrstream oss;
	oss << x << ends;
	string_ = oss.str ();
	delete[] oss.str ();
#endif
	latin1Rep_ = (char *)(0);
}

RgiString::
RgiString (unsigned long x, const RgiFormat &format)
{
#if defined (USE_ASCII)
	Assert(0);
	ostrstream oss;
	oss << x << ends;
	string_ = oss.str ();
#elif defined(USE_QSTRING)
	char fbuf[20];
	sprintf(fbuf,"%%%sd",format.fmt_);
	string_ = new QString();
	string_->sprintf (fbuf, x);
#elif defined(USE_STRING)
	Assert(0);
	ostrstream oss;
	oss << x << ends;
	string_ = oss.str ();
	delete[] oss.str ();
#endif
	latin1Rep_ = (char *)(0);
}

RgiString::
RgiString (double x, const RgiFormat &format)
{
#if defined (USE_ASCII)
	Assert(0);
	ostrstream oss;
	oss << x << ends;
	string_ = oss.str ();
#elif defined(USE_QSTRING)
	char fbuf[20];
	sprintf(fbuf,"%%%sf",format.fmt_);
	string_ = new QString();
	string_->sprintf (fbuf, x);
#elif defined(USE_STRING)
	Assert(0);
	ostrstream oss;
	oss << x << ends;
	string_ = oss.str ();
	delete[] oss.str ();
#endif
	latin1Rep_ = (char *)(0);
}

RgiString::
RgiString (const char *x)
{
#if defined (USE_ASCII)
	if (x) {
		string_ = new char[strlen (x) + 1];
		//strcpy (string_, x);
		strcpy_s(string_, strlen(x) + 1, x);
	} else {
		string_ = (char *)0;
	}
#elif defined(USE_QSTRING)
	string_ = new QString();
	string_->sprintf ("%s", x);
#elif defined(USE_STRING)
	string_ = x;
#endif
	latin1Rep_ = (char *)(0);
}

RgiString::
RgiString (const RgiString &x)
{
#if defined (USE_ASCII)
	if (x.string_) {
		string_ = new char[strlen(x.string_) + 1];
		//strcpy (string_, x.string_);
		strcpy_s(string_, strlen(x.string_) + 1, x.string_);
	} else {
		string_ = (char *)0;
	}
#elif defined(USE_QSTRING)
	string_ = new QString();
	*string_ = *x.string_;
#elif defined(USE_STRING)
	string_ = x.string_;
#endif
	latin1Rep_ = (char *)(0);
}

RgiString::
~RgiString()
{
#if defined(USE_QSTRING)
	delete string_;
	string_ = (QString *)0;
#endif

#if defined (USE_ASCII)
#else

	if (latin1Rep_ != (char *)(0)) {
		delete latin1Rep_;
	}

#endif

	latin1Rep_ = (char *)(0);
}

RgiString &RgiString::
operator = (const RgiString &x)
{
#if defined (USE_ASCII)
	delete[] string_;
	if (x.string_) {
		string_ = new char[strlen(x.string_) + 1];
		//strcpy (string_, x.string_);
		strcpy_s (string_, strlen(x.string_) + 1, x.string_);
	} else {
		string_ = (char *)0;
	}
#elif defined(USE_QSTRING)
	*string_ = *x.string_;
#elif defined(USE_STRING)
	string_ = x.string_;
#endif

	return *this;
}

BOOL RgiString::
operator == (const RgiString &x) const
{
#if defined (USE_ASCII)
	if (!string_ && !x.string_) {
		return TRUE;
	} else if (!string_ || !x.string_) {
		return FALSE;
	} else {
		return !strcmp (string_, x.string_);
	}
#elif defined(USE_QSTRING)
	return *string_ == *x.string_;
#elif defined(USE_STRING)
	return string_ == x.string_;
#endif
}

int RgiString::
operator [] (int idx) const
{
#if defined (USE_ASCII)
	// TODO: this is slow ...
	assert ((idx >= 0) || (idx < length ()));
	return string_[idx];
#elif defined(USE_QSTRING)
#if QT_VERSION < 200
	return (*string_)[idx];
#else
	return (*string_)[idx].latin1();
#endif
#elif defined(USE_STRING)
	return string_[idx];
#endif
}

int RgiString::
length () const
{
#if defined (USE_ASCII)
	if (!string_) {
		return 0;
	}
	return strlen (string_);
#elif defined(USE_QSTRING)
	return string_->length ();
#elif defined(USE_STRING)
	return string_.length ();
#endif
}

BOOL RgiString::
isNull () const
{
#if defined (USE_ASCII)
	if (!string_) {
		return TRUE;
	}
	return FALSE;
#elif defined(USE_QSTRING)
	return string_->isNull();
#elif defined(USE_STRING)
	return string_.isNull();
#endif
}

void RgiString::
empty ()
{
#if defined (USE_ASCII)
	Assert(0);
#elif defined(USE_QSTRING)
	string_->truncate(0);
#elif defined(USE_STRING)
	string_ = RgiString();
#endif
}

BOOL RgiString::
isEmpty () const
{
#if defined (USE_ASCII)
	if (strlen(string_) == 0) {
		return TRUE;
	}
	return FALSE;
#elif defined(USE_QSTRING)
	return string_->isEmpty();
#elif defined(USE_STRING)
	return string_.isEmpty();
#endif
}

RgiString RgiString::
replace (int start, int end, const RgiString &insert) const
{
	assert ((0 <= start) && (start <= end) && (end <= length()));
#if defined (USE_ASCII)
	int newLength = length() + insert.length() + start - end + 1;
	RgiString result;
	result.string_ = new char [newLength];
	// TODO: not the most efficient implementation
	//strncpy (result.string_, string_, start);
	strncpy_s (result.string_, newLength, string_, start);
	//strcpy (result.string_ + start, insert.string_);
	strcpy_s (result.string_ + start, newLength - start, insert.string_);
	//strcat (result.string_, string_ + end);
	strcat_s (result.string_, newLength, string_ + end);
	return result;
#elif defined(USE_QSTRING)
	RgiString result (*this);
	result.string_->replace (start, end-start, *insert.string_);
	return result;
#elif defined(USE_STRING)
	RgiString result (*this);
	result.string_.replace (start, end-start, insert.string_);
	return result;
#endif
}

int RgiString::
contains (const RgiString &str, BOOL cs) const
{
#if defined (USE_ASCII)
	Assert(0);
	int cnt=0;
	return cnt;
#elif defined(USE_QSTRING)
	return string_->contains((const QString &)str,cs);
#elif defined(USE_STRING)
	Assert(0);
	int cnt=0;
	return cnt;
#endif
}

RgiString RgiString::
upper () const
{
#if defined (USE_ASCII)
	if (!string_) {
		return (const char *)0;
	}
//	Assert (0);
	return _strdup(string_);
#elif defined(USE_QSTRING)
	return string_->upper();
#elif defined(USE_STRING)
	return string_.upper();
#endif
}

RgiString RgiString::
lower () const
{
#if defined (USE_ASCII)
	if (!string_) {
		return (const char *)0;
	}
//	Assert (0);
	return _strdup(string_);
#elif defined(USE_QSTRING)
	return string_->lower();
#elif defined(USE_STRING)
	return string_.upper();
#endif
}

RgiString &RgiString::
append (const RgiString &str)
{
#if defined (USE_ASCII)

	int len1 = string_ ? strlen(string_) : 0;
	int len2 = str.string_ ? strlen(str.string_) : 0;

	char *s = new char[len1 + len2 + 1];

	//strncpy(s, string_, len1);
	strncpy_s(s, len1 + len2 + 1, string_, len1);
	//strncpy(s+len1, str.string_, len2);
	strncpy_s(s+len1, len2 + 1, string_, len1);

	delete[] string_;
	string_ = s;
	
	return *this;

#elif defined(USE_QSTRING)

	string_->append(str);

	return *this;

#elif defined(USE_STRING)

	string_.append(str);

	return *this;

#endif
}

//BOOL RgiString::
BOOL RgiString::
operator != (const RgiString &x) const
{
	return ! (*this == x);
}

#if defined (USE_ASCII)
RgiString &RgiString::
operator = (const char *x)
{
	delete[] string_;
	if (x) {
		string_ = new char[strlen(x) + 1];
		//strcpy (string_, x);
		strcpy_s (string_, strlen(x) + 1, x);
	} else {
		string_ = (char *)0;
	}
	return (*this);
}

RgiString::
operator const char *() const
{
	return string_;
}

#elif defined(USE_QSTRING)

RgiString &RgiString::
operator = (const QString &x)
{
	*string_ = x;
	return *this;
}

RgiString::
operator const QString &() const
{
	return *string_;
}

#elif defined(USE_STRING)
RgiString &RgiString::
operator = (const string &x)
{
	string_ = x;
	return *this;
}

RgiString::
operator const string &() const
{
	return string_;
}
#endif

int RgiString::
toInt (BOOL *ok) const
{
#if defined(USE_ASCII)
	if (ok != (BOOL *)0) {
		*ok = TRUE;
	}
	return atoi(string_);
#elif defined(USE_QSTRING)
	bool succ = FALSE;
	int r = string_->toInt (&succ);
	if (ok != (BOOL *)0) {
		*ok = succ;
	}
	return r;
#else
#error NI
#endif
}

double RgiString::
toDouble (BOOL *ok) const
{
#if defined(USE_ASCII)
	*ok = TRUE;
	return atof(string_);
#elif defined(USE_QSTRING)
	bool succ;
	double d = string_->toDouble(&succ);
	if (ok != (BOOL *)0) {
		*ok = succ;
	}
	return d;
#else
#error NI
#endif
}

const char *RgiString::
latin1 () const
{
	//
	// Ugly hack to get around const.
	//

	RgiString *tStr = (RgiString*)(this);
	tStr->findLatin1Rep();

	return latin1Rep_;
}

void RgiString::
findLatin1Rep(void)
{
	int latin1Len = 0;

	if (latin1Rep_ != (char *)(0)) {
		latin1Len = strlen(latin1Rep_) + 1;
	}

#if defined(USE_ASCII)

	latin1Rep_ = string_;

#elif defined(USE_QSTRING)

	const char *tmp = string_->latin1();
	if (tmp != (char *)(0)) {
		
		int tmpLen = strlen(tmp) + 1;
		if (tmpLen > latin1Len) {
			delete latin1Rep_;
			latin1Rep_ = new char [tmpLen];
		}
		strcpy (latin1Rep_, tmp);
	}
	else {

		delete latin1Rep_;
		latin1Rep_ = (char *)(0);
	}

#elif defined(USE_STRING) || defined(USE_WSTRING)

	const char *tmp = string_->c_str();
	if (tmp != (char *)(0)) {
		
		int tmpLen = strlen(tmp) + 1;
		if (tmpLen > latin1Len) {
			delete latin1Rep_;
			latin1Rep_ = new char [tmpLen];
		}
		strcpy (latin1Rep_, tmp);
	}
	else {

		delete latin1Rep_;
		latin1Rep_ = (char *)(0);
	}

#endif
}

ostream &
operator << (ostream &os, const RgiString &str)
{
#if defined(USE_ASCII)
	if ((const char *)str) {
		os << (const char *)str;
	}
#elif defined(USE_QSTRING)
#if QT_VERSION < 200
	os << *str;
#else
	os << ((const QString &)(str)).ascii();
#endif
#elif defined(USE_STRING)
	os << (const string &)str;
#endif
	return os;
}

// - RgiFormat -------------------------------------------------

RgiFormat::
RgiFormat(const char *fmt)
{
	const char *ptr=fmt;

	Assert(fmt && *fmt);

	for (ptr=fmt; *ptr; ptr++) {
		Assert(strchr("0123456789.",*ptr));
	}

	fmt_ = new char[strlen(fmt)+1];
	//strcpy(fmt_,fmt);
	strcpy_s(fmt_, strlen(fmt) + 1, fmt);
}

RgiFormat::
~RgiFormat()
{

	delete fmt_;
}


RgiStringAdaptor::
RgiStringAdaptor(const RgiCString &x)
{
	RgiString tmp(x.data());

	string_=tmp;
}
