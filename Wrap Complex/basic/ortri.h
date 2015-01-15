// Ortri.h -- An oriented triangle class
//

#ifndef __RGI_ORTRI_H__
#define __RGI_ORTRI_H__

#include <basic/dumpable.h>

/*
 * Class OrTri
 *
 * This class represents an oriented triangle.  It consists of two parts:
 * a triangle index and a version (a number between 0 and 5) stored in
 * three bits.  It is synonymous with an integer.
 *
 */

class OrTri
{
public:
  inline OrTri(FIndex ix, int version);
  inline OrTri(void);

  inline FIndex index (void) const;
  inline unsigned version (void) const;
  inline OrTri sym (void) const;
  inline OrTri enext (void) const;
  inline OrTri enext2 (void) const;

  inline unsigned getIntRep (void) const;
  inline void setIntRep (int i);

  inline int operator!= (const OrTri & f2) const;
  inline int operator== (const OrTri & f2) const;
  inline int operator< (const OrTri & f2) const;

  friend inline ostream& operator<< (ostream& s, const OrTri& ef);
  inline void binDump (BinOutput& bo);
  inline Bool binInit (BinInput& bi);
public:

  unsigned int ef_;
};

// Definitions for manipulating the bits of an OrTri

#define Div8  >> 3
#define Mul8  << 3
#define Mod8  & 07

/****************************************************************************
 *
 *
 *        class OrTriStack
 *
 */

class OrTriStack : public IterStackDumpable<OrTri>
{
public:
  OrTriStack (int size = 6) // typical edge degree
	  : IterStackDumpable<OrTri> (size) { }
  OrTriStack (IterStack<OrTri>& other) 
	  : IterStackDumpable<OrTri> (other) { }
  ~OrTriStack (void) { }
};

#include <basic/ortri.hpp>

#endif // __ORTRI_H__
