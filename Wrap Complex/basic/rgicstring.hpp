inline const char *RgiCString::
data () const
{
	return string_;
}

inline RgiCString &RgiCString::
append (const char *str)
{
	return *this += str;
}

inline RgiCString &RgiCString::
append (char ch)
{
	return *this += ch;
}

inline RgiCString &RgiCString::
append (const RgiCString &str)
{
	return append (str.data());
}

inline RgiCString &RgiCString::
replace (int pos, int length, const RgiCString &str)
{
	return replace (pos, length, str.data());
}

inline RgiCString &RgiCString::
replace (int pos, int length, char ch)
{
	return replace (pos, length, RgiCString (ch));
}

inline RgiCString &RgiCString::
insert (int pos, const char *str)
{
	return replace (pos, 0, str);
}

inline RgiCString &RgiCString::
insert (int pos, char ch)
{
	return replace (pos, 0, ch);
}

inline RgiCString &RgiCString::
insert (int pos, const RgiCString &str)
{
	return insert (pos, str.data());
}

inline int RgiCString::
find (const RgiCString &str) const
{
	return find (str.data());
}

inline int RgiCString::
findLast (const RgiCString &str) const
{
	return findLast (str.data());
}

inline RgiCString &RgiCString::
operator += (const RgiCString &str)
{
	return *this += str.data();
}

inline RgiCString &RgiCString::
operator = (const RgiCString &str)
{
	return *this = str.data();
}

inline BOOL RgiCString::
operator > (const char *str) const
{
	return ! (*this <= str);
}

inline BOOL RgiCString::
operator >= (const char *str) const
{
	return ! (*this < str);
}

inline BOOL RgiCString::
operator < (const RgiCString &str) const
{
	return *this < str.data();
}

inline BOOL RgiCString::
operator <= (const RgiCString &str) const
{
	return *this <= str.data();
}

inline BOOL RgiCString::
operator > (const RgiCString &str) const
{
	return *this > str.data();
}

inline BOOL RgiCString::
operator >= (const RgiCString &str) const
{
	return *this >= str.data();
}

inline BOOL RgiCString::
operator == (const RgiCString &str) const
{
	return *this == str.data();
}

inline BOOL RgiCString::
operator != (const char *str) const
{
	return ! (*this == str);
}

inline BOOL RgiCString::
operator != (const RgiCString &str) const
{
	return *this != str.data();
}

inline RgiCString &RgiCString::
erase (int pos, int length)
{
	return replace (pos, length, null);
}

inline RgiCString RgiCString::
toLower () const
{
	return RgiCString(*this).lower();
}

inline RgiCString RgiCString::
toUpper () const
{
	return RgiCString(*this).upper();
}

inline RgiCString RgiCString::
flipped () const
{
	return RgiCString(*this).flip();
}

inline char RgiCString::
operator[] (int pos) const
{
	Assert (string_);
	Assert (pos < (int)strlen(string_));
	return string_[pos];
}

inline char &RgiCString::
operator[] (int pos)
{
	Assert (string_);
	Assert (pos < (int)strlen(string_));
	return string_[pos];
}
