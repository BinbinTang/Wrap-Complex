#ifdef WIN32
#include <strstream>
#else
#include <strstream.h>
#endif

#include <ctype.h>
#ifdef WIN32
#else
#include <strings.h>
#endif
#include <cstdlib>
#include "rgicstring.h"

static inline int
length (const char *str)
{
	if (str == (char *)(0)) {
		return 0;
	}
	return strlen (str);
}

RgiCString RgiCString::null;

RgiCString::
RgiCString ()
{
	string_ = (char *)0;
}

RgiCString::
RgiCString (char x)
{
	string_ = new char[2];
	string_[0] = x;
	string_[1] = '\0';
}

RgiCString::
RgiCString (int x)
{
	ostrstream oss;
	oss << x << ends;
	string_ = oss.str();
}

RgiCString::
RgiCString (float x)
{
	ostrstream oss;
	oss << x << ends;
	string_ = oss.str();
}

RgiCString::
RgiCString (const double x)
{
	ostrstream oss;
	oss << x << ends;
	string_ = oss.str();
}

RgiCString::
RgiCString (const char *string)
{
	if (string != (char *)(0)) {
		string_ = new char [strlen (string) + 1];
		//strcpy (string_, string);
		strcpy_s (string_, strlen(string) + 1, string);
	} else {
		string_ = (char *)0;
	}
}

RgiCString::
RgiCString (const char *string, int len)
{
	if (string != (char *)(0)) {
		Assert (len <= (int)strlen (string));
		string_ = new char [len + 1];
		//strncpy (string_, string, len);
		strncpy_s (string_, len + 1, string, len);
		string_[len] = '\0';
	} else {
		Assert (len == 0);
		string_ = (char *)0;
	}
}

RgiCString::
RgiCString (const RgiCString &str)
{
	if (str.string_ != (char *)(0)) {
		string_ = new char [strlen (str.string_) + 1];
		//strcpy (string_, str.string_);
		strcpy_s (string_, strlen(str.string_) + 1, str.string_);
	} else {
		string_ = (char *)0;
	}
}

RgiCString::
RgiCString (ostrstream &oss)
{
	oss << ends;
	string_ = oss.str();
}

RgiCString::
~RgiCString ()
{
	delete[] string_;
}

BOOL RgiCString::
isEmpty () const
{
	if (string_ == (char *)(0)) {
		return TRUE;
	}
	return string_[0] == '\0';
}

void RgiCString::
empty ()
{
	delete[] string_;
	string_ = (char *)0;
}

int RgiCString::
length () const
{
	if (string_ == (char *)(0)) {
		return 0;
	}
	return strlen (string_);
}

RgiCString &RgiCString::
replace (int pos, int rlen, const char *str)
{
	int len = length();
	int slen = ::length(str);

	Assert (pos >= 0);
	Assert (rlen >= 0);
	Assert (pos + rlen <= len);

	int tempLen = len + slen - rlen + 1;
	char *tmp = new char[tempLen];
	//strncpy (tmp, string_, pos);
	strncpy_s (tmp, tempLen, string_, pos);
	if (str != (char *)(0)) {
		//strcpy (tmp+pos, str);
		strcpy_s (tmp+pos, tempLen - pos, str);
	}
	if (string_ != (char *)(0)) {
		//strcpy (tmp+pos+slen, string_+pos+rlen);
		strcpy_s (tmp+pos+slen, tempLen - pos - slen, string_+pos+rlen);
	}
	delete[] string_;
	string_ = tmp;
	return *this;
}

int RgiCString::
find (const char *str) const
{
	int slen = ::length(str);
	int len = length();

	if (slen > len) {
		return -1;
	}
	if (slen == 0) {
		return 0;
	}
	for (int i=0; i<=len-slen; i++) {
		if (string_[i]==str[0] && !strncmp (string_+i, str, slen)) {
			return i;
		}
	}
	return -1;
}

int RgiCString::
find (char ch) const
{
	if (isEmpty()) {
		return -1;
	}
	char *ptr = strchr (string_, ch);
	if (!ptr) {
		return -1;
	}
	return ptr - string_;
}

int RgiCString::
findLast (const char *str) const
{
	int slen = ::length(str);
	int len = length();

	if (slen > len) {
		return -1;
	}
	if (slen == 0) {
		return len;
	}
	for (int i=len-slen; i>=0; i--) {
		if (string_[i]==str[0] && !strncmp (string_+i, str, slen)) {
			return i;
		}
	}
	return -1;
}

int RgiCString::
findLast (char ch) const
{
	if (isEmpty()) {
		return -1;
	}
	char *ptr = strrchr (string_, ch);
	if (!ptr) {
		return -1;
	}
	return ptr - string_;
}

RgiCString RgiCString::
sub (int first, int len) const
{
	Assert (first >= 0);
	Assert (len >= 0);
	Assert (first + len <= length());

	return RgiCString (string_+first, len);
}

int RgiCString::
toInt () const
{
	if (string_ == (char *)(0)) {
		return 0;
	}
	return atoi(string_);
}

double RgiCString::
toDouble () const
{
	if (string_ == (char *)(0)) {
		return 0;
	}
	return atof(string_);
}

RgiCString &RgiCString::
lower ()
{
	if (string_ != (char *)(0)) {
		for (char *next=string_; *next; next++) {
			*next = tolower (*next);
		}
	}
	return *this;
}

RgiCString &RgiCString::
upper ()
{
	if (string_ != (char *)(0)) {
		for (char *next=string_; *next; next++) {
			*next = toupper (*next);
		}
	}
	return *this;
}

RgiCString &RgiCString::
flip ()
{
	if (string_ != (char *)(0)) {
		int len = length();
		int mid = len / 2;
		for (int i=0; i<mid; i++) {
			char tmp = string_[i];
			string_[i] = string_[len-1-i];
			string_[len-1-i] = tmp;
		}
	}
	return *this;
}

RgiCString &RgiCString::
operator += (char ch)
{
	int oldLen = length();
	char *tmp = new char [oldLen + 2];
	//strncpy (tmp, string_, oldLen);
	strncpy_s (tmp, oldLen + 2, string_, oldLen);
	tmp[oldLen] = ch;
	tmp[oldLen+1] = '\0';
	delete[] string_;
	string_ = tmp;
	return *this;
}

RgiCString &RgiCString::
operator += (const char *str)
{
	int slen = ::length(str);
	if (slen > 0) {
		int oldLen = length();
		char *tmp = new char [oldLen + ::length(str) + 1];
		//strncpy (tmp, string_, oldLen);
		strncpy_s (tmp, oldLen + ::length(str) + 1, string_, oldLen);
		//strcpy (tmp+oldLen, str);
		strcpy_s (tmp+oldLen, ::length(str) + 1, str);
		delete[] string_;
		string_ = tmp;
	}
	return *this;
}

RgiCString &RgiCString::
operator = (const char *str)
{
	if (str != string_) {
		delete[] string_;
		if (str != (char *)(0)) {
			string_ = new char [::length(str) + 1];
			if (str != (char *)(0)) {
				//strcpy (string_, str);
				strcpy_s (string_, ::length(str) + 1, str);
			} else {
				string_[0] = '\0';
			}
		} else {
			string_ = (char *)0;
		}
	}
	return *this;
}

BOOL RgiCString::
operator == (const char *str) const
{
	if (string_ == (char *)(0) && str == (char *)(0)) {
		return TRUE;
	}
	if (string_ != (char *)(0) || str != (char *)(0)) {
		return FALSE;
	}
	return strcmp (string_, str) == 0;
}

BOOL RgiCString::
operator < (const char *str) const
{
	if (str == (char *)(0)) {
		return FALSE;
	}
	if (string_ == (char *)(0)) {
		return TRUE;
	}
	return strcmp (string_, str) < 0;
}

BOOL RgiCString::
operator <= (const char *str) const
{
	if (str == (char *)(0)) {
		return (string_ == (char *)0);
	}
	if (string_ == (char *)(0)) {
		return TRUE;
	}
	return strcmp (string_, str) <= 0;
}

ostream &
operator<< (ostream &os, const RgiCString &str)
{
	if (str.data()) {
		os << str.data();
	}
	return os;
}
