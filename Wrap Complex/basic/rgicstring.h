#ifndef __RGI_RGICSTRING_H__
#define __RGI_RGICSTRING_H__

#include <basic/basic.h>

#include <fstream>
#include <iostream>
#include <strstream>

//class istream;
//class ostream;
//class ostrstream;

class RgiCString
{
public:
	RgiCString ();
	RgiCString (char);
	RgiCString (int x);
	RgiCString (float x);
	RgiCString (const double x);
	RgiCString (const char *string);
	RgiCString (const char *string, int len);
	RgiCString (ostrstream &oss);
	RgiCString (const RgiCString &);

	~RgiCString ();

	RgiCString &append (const char *);
	RgiCString &append (const RgiCString &);
	RgiCString &append (char);

	RgiCString &insert (int pos, const char *);
	RgiCString &insert (int pos, const RgiCString &);
	RgiCString &insert (int pos, char);

	RgiCString &replace (int pos, int length, const char *);
	RgiCString &replace (int pos, int length, const RgiCString &);
	RgiCString &replace (int pos, int length, char);

	RgiCString &erase (int pos, int length);

	int find (const char *) const;
	int find (const RgiCString &) const;
	int find (char) const;

	int findLast (const char *) const;
	int findLast (const RgiCString &) const;
	int findLast (char) const;

	RgiCString sub (int first, int last) const;

	void empty ();
	BOOL isEmpty () const;
	int length () const;

	RgiCString toLower () const;
	RgiCString toUpper () const;
	RgiCString flipped () const;

	RgiCString &lower ();
	RgiCString &upper ();
	RgiCString &flip ();

	int toInt() const;
	double toDouble() const;

	RgiCString &operator += (char);
	RgiCString &operator += (const RgiCString &);
	RgiCString &operator += (const char *);

	RgiCString &operator = (const char *);
	RgiCString &operator = (const RgiCString &);
	BOOL operator < (const char *) const;
	BOOL operator < (const RgiCString &) const;
	BOOL operator <= (const char *) const;
	BOOL operator <= (const RgiCString &) const;
	BOOL operator > (const char *) const;
	BOOL operator > (const RgiCString &) const;
	BOOL operator >= (const char *) const;
	BOOL operator >= (const RgiCString &) const;
	BOOL operator == (const char *) const;
	BOOL operator == (const RgiCString &) const;
	BOOL operator != (const char *) const;
	BOOL operator != (const RgiCString &) const;

	char operator [] (int) const;
	char &operator [] (int);

	const char *data () const;

	static RgiCString null;

private:
	char *string_;
};

#define rgiCString(X) RgiCString((ostrstream &)(ostrstream()<<X))

ostream &operator<< (ostream &, const RgiCString &);

#include <basic/rgicstring.hpp>

#endif

