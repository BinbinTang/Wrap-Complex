/*--------------------------------------------------------------------------*/
/*
 * NAME: bitvector.h
 *
 * DESCRIPTION:
 *	This file contains type definitions and function prototypes
 *	supporting a bitvector data structure.
 *
 * HISTORY:
 *	89/10/26: This header added
 */

/*--------------------------------------------------------------------------*/

#ifndef __RGI_BITVECTOR_H__
#define __RGI_BITVECTOR_H__

#include <basic/basic.h>
#include <basic/binio.h>

class IMap;

class BitVector 
{
friend class BVTag;
friend class SimplexBVTag;

public:
  enum { scaleFactor_ = 2 };
  typedef unsigned int bvWordType;

  BitVector (int length = 10);
  BitVector (const BitVector & bv);
  const BitVector& operator= (const BitVector & bv);
  ~BitVector()  { free (bvwords_); }

  int operator== (const BitVector & other) const;
  void reset ();
  void resetAssert ();
  void setAll ();
  void setAll (BitVector& other);
  void reverse (BitVector& other);
  int  size () const; 
  Bool set (int i);
  Bool clear (int i);
  Bool set (int i, Bool val);
  Bool test (int i) const;
  void resize (int newSize);
  int getNumSet (void) const;
  int computeNumSet (void);
  int getNextSet (int a);
  int getNextClear (int a);
  int getPrevSet (int a);
  int getPrevClear (int a);
  inline int maxSetIndex (void);
  void binDump (BinOutput& bo);
  Bool binInit (BinInput& bi);
  BitVector& reverse ();
  BitVector& and_op (BitVector& other);
  BitVector& or_op (BitVector& other);
  BitVector& xor_op (BitVector& other);
  BitVector& sub_op (BitVector& other);
  BitVector& neg_op (void);
	void remap (IMap& map);

  // IO

  friend ostream&   operator<< (ostream& s, const BitVector& bv);

private:
  Magic magic_;
  static bvWordType modMask_;
  static bvWordType bitsPerWord_;
  static bvWordType lgbits_;   /* log2(bits per word) */
  bvWordType *bvwords_;
  
  int maxn_;
  int numSet_;
  void adjustSize (int newSize);
  void adjustTo_ (BitVector& other, unsigned int& n, unsigned int& othern);
  inline unsigned int numSetWord_ (bvWordType word);

};

/*--------------------------------------------------------------------------*/
/*                           CLASS BitVectorIter                            */
/*--------------------------------------------------------------------------*/
// set mode is TRUE if go through all set, and FALSE if go through all clear
class BitVectorIter
{
public:
  BitVectorIter (BitVector& bv, Bool setMode = TRUE);
  void first (void);
  void next (void);
  Bool isDone (void);
  int curItem (void);
private:
  BitVector& bv_;
  int curIndex_;
  int (BitVector::*getNext_) (int); 
};

/*--------------------------------------------------------------------------*/

#include <basic/bitvector.hpp>


#endif /* __BITVECTOR_H__ */

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
