/*
 * Implementation of long integers
 *
 *
 *
Solution for implementation of long integers:

FILL THIS IN...

 */

#ifndef __RGI_LI_H__
#define __RGI_LI_H__

#include <iostream>
#include <basic/miscmath.h>
#include <li/lia.h>
#include <li/base.h>

class BinInput;
class BinOutput;
class RealPArray;

#define INT_BYTES   32
#define DO_INLINE true

#ifdef DO_INLINE
#define INLINE inline
#else
#define INLINE
#endif

// Define the variable below if you want all LI operations to allow
// the same operands to occur in any positions of an operation.  This
// is a bit more expensive...
//
// #define SAME_OPS 1

#define PRINT(X)  ;
// #define PRINT(X)  cout << X


class LI_Coord;

//--------------------------------------------------------------------------

//. CLASS LI
//-
//- This class implements long integers, along with arithmetic and I/O
//- operations on them.


class LIBase
{
public:
  enum { LI_SHORT = 0, LI_LONG = 1 };
  INLINE void negate (void);
  INLINE int  sign   (void);
  INLINE double real (void);
  Lia_obj lia_obj_;
protected: 
  LIBase (void) : lia_obj_(NULL) { }
  ~LIBase (void) { delete [] lia_obj_; }
};

ostream&   operator<< (ostream& s, const LIBase& li);

class LIShort
{
public:
  enum { LI_SIZE = LIBase::LI_SHORT };
};

class LILong
{
public:
  enum { LI_SIZE = LIBase::LI_LONG };
};

template <class LISpec>
class LIGen : public LIBase
{
public:

  /* All of the methods below need to be defined for all subclasses of
     this class. */
  static void setSize  (int size);
  static int getSize (void);

  LIGen (void);
  LIGen (const LIBase &other);
  LIGen (int num);
  LIGen (char *num);
  LIGen<LISpec> const &operator= (const LIBase &lnum);

  void alloc ();
  void free ();
 // virtual int  getMaxSize (void);

  // Alternative implementations of arithmetic operations.  Each
  // one performs the indicated operation and stores the result in
  // this object.  If this object is the same as one of the two operands,
  // then some intermediate storage place is used.

  INLINE void add (const LIBase &other1, const LIBase &other2);
  INLINE void sub (const LIBase &other1, const LIBase &other2);
  INLINE void mult (const LIBase &other1, const LIBase &other2);
  void operator+= (const LIBase &other);
  void operator-= (const LIBase &other);
  void operator*= (const LIBase &other);

protected:
  static int size_;

};

class LI_Coord;

class LI : public LIGen<LILong>
{
public:
  LI (void) { }
  LI (const LIBase &other) : LIGen<LILong> (other) { }
  LI (int num) : LIGen<LILong> (num) { }
  LI (char *num) : LIGen<LILong> (num) { }
  LI const &operator= (const LI &lnum);
  LI const &operator= (const LI_Coord &lnum);
  static void validatePrecision (int& lMax, int& pMax);
};

//--------------------------------------------------------------------------

//. CLASS LI_Coord
//-
//- This class implements long integers for representing input coordinates.
//- It should only be used within objects storing coordinates (such as
//- point arrays).  All mathematical operations should be performed on
//- LI objects only, since LI_Coord objects can be converted into a LI.
//- This also means that LI_Coord should *never* be used as temporary
//- storage.  An LI_Coord can be assigned a LI, and will do size checking
//- to make sure that this is valid.

class LI_Coord : public LIGen<LIShort>
{
public:
  LI_Coord (void) { }
  LI_Coord (const LIBase &other) : LIGen<LIShort> (other) { }
  LI_Coord (int num) : LIGen<LIShort> (num) { }
  LI_Coord (char *num) : LIGen<LIShort> (num) { }
  Bool binInit(BinInput& bi);
  LI_Coord const &operator= (const LI_Coord &lnum);
  LI_Coord const &operator= (const LI &lnum);
  void binDump (BinOutput& bo);
};


LI operator+ (const LIBase &other1, const LIBase &other2);
LI operator- (const LIBase &other1, const LIBase &other2);
LI operator* (const LIBase &other1, const LIBase &other2);
LI operator- (const LIBase &other);

INLINE int operator== (const LIBase &other1, const LIBase &other2);
INLINE int operator!= (const LIBase &other1, const LIBase &other2);
INLINE int operator<  (const LIBase &other1, const LIBase &other2);
INLINE int operator<= (const LIBase &other1, const LIBase &other2);
INLINE int operator>  (const LIBase &other1, const LIBase &other2);
INLINE int operator>= (const LIBase &other1, const LIBase &other2);


inline LI operator+ (const LI &other1, const LI &other2)
{
  LI tmp;

  Assert ((other1.lia_obj_ != NULL) && (other2.lia_obj_ != NULL));
  tmp.alloc();
  lia_add (tmp.lia_obj_, other1.lia_obj_, other2.lia_obj_);

  return tmp;
}

//--------------------------------------------------------------------------

//. CLASS LI_Converter
//-
//- This class will convert various types of numeric input into long
//- integers.  The types are:  integers, integer strings (arbitrary
//- length integers stored in strings), and fixed point numbers stored
//- either in floats, doubles or strings (arbitrary length floating point
//- numbers).  This converter will construct LI_Coords for representing
//- input points.

class LI_Point;

class LI_Converter
{
public:
  LI_Converter (int w = 10, int a = 0);
  LI_Converter (LI_Converter& other);
	LI_Converter& operator= (const LI_Converter& other);

  void toLI (int, LIBase &);
  void toLI (float, LIBase &);
  void toLI (double, LIBase &);
  void toLI (char *, LIBase &);

  // 
  void toLi (double x[3], int computeSumSq, LI_Point &);
	// truncated multiplied double point to LI_Point
	void truncMulToLi (double x[3], int computeSumSq, LI_Point &);

  double toReal (LIBase&);
  char *toString (LIBase&);
  int getFixedW (void);
  int getFixedA (void);
  double getScale (void);
	void setPrecision (int lMax, int pMax);
	void getPrecision (int& lMax, int& pMax);
  // multiply and truncate
  
  // "integer" -> truncated "integer"
  void trunc (double x[4]);
  // "double" -> truncated "integer"
  void truncMul (double x[4]);
  // "double"  ->truncated "double"
  void truncMulDiv (double x[4]);

  void truncPArray (RealPArray& rArray);
  
  // NOTE:  The above conversion function currently returns numbers in
  // exponential form (ddddde-ddd).  This can be changed if necessary...
private:
	void truncateD_ (double x[4]);
  void truncateF_ (double x[4]);
	void truncateI9_1_ (double x[4]);
	void truncateI9_2_ (double x[4]);
	void (LI_Converter::*truncate_) (double *);
	void init_ (void);
  int fixed_w_;
  int fixed_a_;
  double scale_;
};


#ifdef DO_INLINE
#include "li/li.hpp"
#endif

#endif /* __LI_H__ */
