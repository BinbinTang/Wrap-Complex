
#ifndef __RGI_BVTAG_H__
#define __RGI_BVTAG_H__

#include <basic/dumpable.h>
#include <basic/bitvector.h>
#include <geometry/simplex.h>

/************************************************************************/

/*
 * Class BitVectorPool
 *
 *
 *
 */

class BitVectorPool : public IterStack<BitVector *>
{
public:
  BitVectorPool (void);
  void put (BitVector *bv) { Assert ((bv->resetAssert(), 1)); push (bv); }
  BitVector *get (void) {return empty()? new BitVector(): pop(); } 
  ~BitVectorPool (void) { while (!empty()) delete pop (); }
};



/************************************************************************/

/*
 * Class BVTag;
 *
 *
 *
 */


class BVTag
{
public:
  BVTag (BitVectorPool& bvpool, int size=10);             
  BVTag (const BVTag& bvtag);
  const BVTag& operator=(const BVTag& bvtag);
  friend ostream&   operator<< (ostream& s, const BVTag& bvtag);
  ~BVTag (void);
               
  void reset(void);
  void set(int i);
  void clear(int i);
  int  test(int i) const;
  void set (Simplex sigma);
  void clear (Simplex sigma);
  int  test (Simplex sigma) const;
  int getNumSet (void) const;
  BitVector& getBitVector (void);
  BitVectorPool& getBVPool (void);
private:
  BitVectorPool& bvpool_;
  BitVector *bv_;
};

/************************************************************************/

/*
 * Class BVTagIter
 *
 *
 *
 */

class BVTagIter
{
public:
  BVTagIter (BVTag& tag);
  ~BVTagIter (void);
  void first (void);
  void next (void);
  Bool isDone (void);
  int curItem (void);
private:
  BitVectorIter iter_;
};

/************************************************************************/

/*
 * Class SimplexBVTag
 *
 *
 *
 */

class SimplexBVTag
{
public:
  SimplexBVTag (BitVectorPool& bvpool, Dim dim=3, int size=10); 
  SimplexBVTag (const SimplexBVTag&);
  friend ostream&   operator<< (ostream& s, const SimplexBVTag& bvtag);
  SimplexBVTag& operator= (SimplexBVTag&);
 ~SimplexBVTag (void); 
  Dim getDim (void);
  void reset (Dim d);
  void reset (void);
  void set (Dim d, int i);
  void clear (Dim d, int i);
  int  test (Dim d, int i) const;
  void set (Simplex sigma);
  void clear (Simplex sigma);
  int  test (Simplex sigma) const;
private:
  BitVectorPool& bvpool_;
  BitVector *bv_[4];
  Dim dim_;
};

// BVTag inline functions
inline BVTag::BVTag (BitVectorPool& bvpool, int size) 
                   : bvpool_(bvpool),
                   bv_ (bvpool.get()) { }
inline BVTag::BVTag (const BVTag& bvtag)
        : bvpool_(bvtag.bvpool_),
          bv_(bvtag.bvpool_.get()) 
        { *bv_ = *bvtag.bv_; }
inline const BVTag& BVTag::operator=(const BVTag& bvtag) 
        { *bv_ = *bvtag.bv_; return *this; }
inline void BVTag::reset(void) { bv_->reset (); }
inline int BVTag::getNumSet (void) const { return bv_->getNumSet (); }
inline BVTag::~BVTag (void) { if (getNumSet()) reset(); bvpool_.put(bv_); }              
inline void BVTag::set(int i) { bv_->set (i); }
inline void BVTag::clear(int i) { bv_->clear (i); }
inline int  BVTag::test(int i) const {return bv_->test (i); }
inline void BVTag::set (Simplex sigma) { bv_->set (sigma.getIndex ());   }
inline void BVTag::clear (Simplex sigma) { bv_->clear (sigma.getIndex ()); }
inline int  BVTag::test (Simplex sigma) const { return bv_->test (sigma.getIndex ()); }
inline BitVector& BVTag::getBitVector (void) { return *bv_; }
inline BitVectorPool& BVTag::getBVPool (void) { return bvpool_; }

// BVTagIter inline functions
inline BVTagIter::BVTagIter (BVTag& tag)
       : iter_(tag.getBitVector()) { }
inline BVTagIter::~BVTagIter (void) { }
inline void BVTagIter::first (void) { iter_.first(); }
inline void BVTagIter::next (void) { iter_.next(); }
inline Bool BVTagIter::isDone (void) { return iter_.isDone(); }
inline int BVTagIter::curItem (void) { return iter_.curItem(); }

// SimplexBVTag inline functions
inline Dim SimplexBVTag::getDim (void) { return dim_; }
inline void SimplexBVTag::reset (Dim d) { bv_[d]->reset ();}
inline void SimplexBVTag::reset (void)  { for (int d = 0; d <= dim_; d++) reset (d);  }
inline void SimplexBVTag::set (Dim d, int i)   { bv_[d]->set (i);   }
inline void SimplexBVTag::clear (Dim d, int i) { bv_[d]->clear (i); }
inline int SimplexBVTag::test (Dim d, int i) const { return bv_[d]->test (i); }
inline void SimplexBVTag::set (Simplex sigma) 
            { bv_[sigma.getDim ()]->set (sigma.getIndex ());   }
inline void SimplexBVTag::clear (Simplex sigma)
            { bv_[sigma.getDim ()]->clear (sigma.getIndex ()); }
inline int  SimplexBVTag::test (Simplex sigma) const 
            { return bv_[sigma.getDim ()]->test (sigma.getIndex ()); }

#endif // __BVTAG_H__
