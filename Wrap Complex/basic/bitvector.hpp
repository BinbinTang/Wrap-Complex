/*
 * bitvector.hpp -- Inlined functions for bitvectors
 *
 */

inline int BitVector::
getNumSet (void) const
{
  if (numSet_ == -1)
  {
      BitVector *myself = (BitVector*) this;
      return myself->computeNumSet ();
  }
  return numSet_;
}


/*--------------------------------------------------------------------------*/

inline Bool BitVector::
set(int i)
{
  assert (i >= 0);
  
  if (i > maxn_)
    adjustSize (i);

  unsigned int word = i >> lgbits_;
  bvWordType setbit = 1 << (i & modMask_);

  if (bvwords_[word] & setbit)
    return TRUE;

  bvwords_[word] |= setbit;

  if (numSet_ != -1)
    numSet_++;
  return TRUE;
}

/*--------------------------------------------------------------------------*/

inline Bool BitVector::
clear(int i)
{
  assert (i >= 0);
  
  if (i > maxn_)
    return TRUE;

  unsigned int word = i >> lgbits_;
  bvWordType   setbit = 1 << (i & modMask_);

  if (!(bvwords_[word] & setbit))
    return TRUE;

  bvwords_[word] ^= setbit;

  if (numSet_ != -1)
    numSet_--;
  return TRUE;
}

inline Bool BitVector::
set (int i, Bool val)
{
  return val? set(i) : clear(i);
}

/*--------------------------------------------------------------------------*/

inline int BitVector::
test (int i) const
{
  if (i > maxn_) 
    return 0;
  assert (i >= 0);

  return ((bvwords_[i >> lgbits_] & (1 << (i & modMask_))) != 0);
}

/*--------------------------------------------------------------------------*/

inline int BitVector::
size (void) const
{
  return maxn_;
}

/*--------------------------------------------------------------------------*/

inline void BitVector::
resize (int newSize)
{
  adjustSize (newSize);
}

/*--------------------------------------------------------------------------*/

inline unsigned int BitVector::
numSetWord_ (bvWordType word)
{
  unsigned int val = 0;
  bvWordType one = 0x1;
  for( unsigned int i=0; i<bitsPerWord_; i++)
  {
    val += one & word;
    word >>= 1;
  }
  return val;
}

/*---------------------------------------------------------------------------*/

inline int BitVector::
maxSetIndex (void)
{
  return getPrevSet(maxn_+1);
}


/*---------------------------------------------------------------*/

inline BitVectorIter::
BitVectorIter (BitVector& bv, Bool setMode)
: bv_(bv), curIndex_(-1),
getNext_(setMode? &BitVector::getNextSet : &BitVector::getNextClear)
{

}

//------------------------------------------------------------------

inline void BitVectorIter::
first (void)
{
  curIndex_ = (bv_.*getNext_)(-1);
}

//------------------------------------------------------------------

inline void BitVectorIter::
next (void)
{
  curIndex_ = (bv_.*getNext_)(curIndex_);
}

//------------------------------------------------------------------

inline Bool BitVectorIter::
isDone (void)
{
  return (curIndex_ == -1);
}  

//------------------------------------------------------------------

inline int BitVectorIter::
curItem (void)
{
  return curIndex_;
}
