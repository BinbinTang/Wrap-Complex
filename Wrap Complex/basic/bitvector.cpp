
#include <iostream>
/*
#include <malloc.h>
#include <math.h>
*/

#include <basic/basic.h>
#include <basic/bitvector.h>
#include <basic/dumpable.h>

#define BITVECTOR_MAGIC 828282838

static BitVector::bvWordType getLgBits (void)
{
  BitVector::bvWordType lgbits = 0;
  BitVector::bvWordType  tmp = sizeof(BitVector::bvWordType) << 3;
    while (tmp >>= 1)
      lgbits++;
  return lgbits;
}

BitVector::bvWordType BitVector::modMask_ = (sizeof(bvWordType)<<3) - 1;

BitVector::bvWordType BitVector::bitsPerWord_ = sizeof(bvWordType) << 3;

BitVector::bvWordType BitVector::lgbits_ = getLgBits();

/*---------------------------------------------------------------------------*/

// extern void *malloc(), *calloc();

/*---------------------------------------------------------------------------*/

// maxn_ is maximum index currently available

BitVector::BitVector(int max): magic_(BITVECTOR_MAGIC)
{
  unsigned int nwords = (max>>lgbits_) + 1;
  maxn_ = (nwords<<lgbits_) - 1;
  numSet_ = 0;
  
  bvwords_ = (bvWordType *) malloc(sizeof(bvWordType)*nwords);
  BZERO (bvwords_, bvWordType, nwords);
}

/*--------------------------------------------------------------------------*/

BitVector::
BitVector (const BitVector & bv): magic_(BITVECTOR_MAGIC)
{
  unsigned int nwords = (bv.maxn_>>lgbits_) + 1;
  maxn_ = (nwords<<lgbits_) - 1;
  numSet_ = bv.numSet_;
  
  bvwords_ = (bvWordType *) malloc(sizeof(bvWordType)*nwords);
  BCOPY (bv.bvwords_, bvwords_, sizeof(bvWordType)*nwords);
}

/*--------------------------------------------------------------------------*/

Bool BitVector::
binInit (BinInput& bi)
{
  if (!magic_.binInit(bi))
    return FALSE;
  unsigned int nwords = bi.readUnsignedInt();
  maxn_ = bi.readInt();
  /* int mode = */ bi.readInt();
  numSet_ = bi.readInt();
  
  assert (nwords == ((maxn_ >>lgbits_) + 1)); 
  
  bvwords_ = (bvWordType *) realloc((char *) bvwords_, sizeof(bvWordType)*nwords);

  bi.readUnsignedInts(bvwords_, nwords); 
  
  return TRUE;
}

/*---------------------------------------------------------------------------*/

int BitVector::
operator== (const BitVector & other) const
{
  unsigned int i;
  unsigned int nwords = (maxn_>>lgbits_) + 1;
  unsigned int othernwords = (other.maxn_>>lgbits_) + 1;
  unsigned int minwords = (nwords < othernwords)? nwords : othernwords;
  int equal = TRUE;

  i = 0;
  while ((i < minwords) && equal)
    {
      equal = (bvwords_[i] == other.bvwords_[i]);
      i++;
    }
  
  while ((i < nwords) && equal)
    {
      equal = (bvwords_[i] == 0);
      i++;
    }
  
  while ((i < othernwords) && equal)
    {
      equal = (other.bvwords_[i] == 0);
      i++;
    }
  
  return equal;
}

/*---------------------------------------------------------------------------*/

int BitVector::
computeNumSet (void)
{
  numSet_ = 0;
  BitVectorIter iter(*this);
  for(iter.first(); !iter.isDone(); iter.next())
    numSet_++;

  return numSet_;
}

/*--------------------------------------------------------------------------*/

void BitVector::
binDump (BinOutput& bo)
{
  magic_.binDump(bo);
  unsigned int nwords = (maxn_>>lgbits_) + 1; 
   
  bo.writeUnsignedInt(nwords);
  bo.writeInt(maxn_);
  int mode = 0;
  bo.writeInt(mode);
  bo.writeInt(numSet_);
  bo.writeUnsignedInts(bvwords_, nwords);
  
}
  
/*--------------------------------------------------------------------------*/

const BitVector& 
BitVector::operator= (const BitVector & bv)
{
  if (&bv == this)
     return *this;

  unsigned int bv_nwords = (bv.maxn_>>lgbits_) + 1;
  unsigned int nwords = (maxn_>>lgbits_) + 1; 
  if (bv_nwords > nwords)
  {
    free(bvwords_);
    bvwords_ = (bvWordType *) malloc(sizeof(bvWordType)*bv_nwords);
    maxn_ = bv.maxn_;
  }
  else if (nwords > bv_nwords)
  {
   BZERO (&bvwords_[bv_nwords], bvWordType, nwords - bv_nwords);
  }

   BCOPY (bv.bvwords_, bvwords_, sizeof(bvWordType)*bv_nwords);

  numSet_ = bv.numSet_;
  return *this;
}

/*--------------------------------------------------------------------------*/

void 
BitVector::reset()

{
  if (!numSet_)
    return;
  unsigned int nwords = (maxn_>>lgbits_) + 1;
  maxn_ = (nwords<<lgbits_) - 1;
  BZERO (bvwords_, bvWordType, nwords);
  numSet_ = 0;
}

/*--------------------------------------------------------------------------*/

void 
BitVector::resetAssert()
{
#ifdef __DEBUG__
//  unsigned int nwords = maxn_ / bitsPerWord_ + 1;
  assert(numSet_ == 0);
//  for (unsigned int i=0; i < nwords; i++)
//   {  Assert(bvwords_[i] == 0); }
#endif
}

/*--------------------------------------------------------------------------*/

void 
BitVector::setAll()
{
  unsigned int nwords = (maxn_ >>lgbits_) + 1;
  maxn_ = (nwords<<lgbits_) - 1;
  int minusOne = -1;

  for (unsigned int i=0; i < nwords; i++) bvwords_[i] = (bvWordType) minusOne;
  numSet_ = nwords * bitsPerWord_;
}

/*--------------------------------------------------------------------------*/

void 
BitVector::setAll (BitVector& other)
{
  unsigned int i;
  unsigned int nwords;
  unsigned int othernwords;
  adjustTo_ (other, nwords, othernwords);

  numSet_ = other.numSet_;
  for(i=0; i<othernwords; i++)
    bvwords_[i] = other.bvwords_[i];

  if (nwords > othernwords)
    BZERO (&bvwords_[othernwords], bvWordType, nwords - othernwords);
}

/*--------------------------------------------------------------------------*/

void 
BitVector::reverse (BitVector& other)
{
  // Reverse a bitvector relative to another bitvector.
  // This is exactly an exclusive-or!!

  xor_op (other);
}

/*--------------------------------------------------------------------------*/

ostream&  
operator<< (ostream& s, const BitVector& bv)
{
  int numLine = 0;
  s << "numSet=" << bv.getNumSet() << endl;
  for (int i = 0; i <= bv.maxn_; i++)
    if (bv.test(i))
    {
      if (numLine >= 20)
      {
        s << endl;
        numLine = 0;
      }
      s << i << " ";
      numLine ++;
    }

  s << endl;
  return s;
}


/*--------------------------------------------------------------------------*/

//
// adjustSize (newSize) -- adjusts the size of this bitvector so that the
//                         size is >= newSize.
//

void
BitVector::adjustSize (int minNewSize)
{
  static char *fail = { "bvNew: fail on [mc]alloc.\n" };
  int newN;

  if (minNewSize < maxn_) // reduce the size
    newN = minNewSize;
  else // increase the size
  {
    newN = maxn_;
    while (newN <= minNewSize)
      newN *= scaleFactor_;
  }

  int nwordsBefore = (maxn_ >>lgbits_) + 1;
  int nwordsAfter = (newN >>lgbits_) + 1;

  bvWordType * oldbvwords = bvwords_;

    bvwords_ = (bvWordType *) 
    realloc ((char *) oldbvwords, nwordsAfter  * sizeof (bvWordType));


  if (bvwords_ == NULL) {
    (void) fprintf(stdout, fail);
    assert(0);
  }
   
  if (nwordsAfter > nwordsBefore)
  {
    BZERO (&bvwords_[nwordsBefore], bvWordType, nwordsAfter - nwordsBefore);
  }

  maxn_ = (nwordsAfter <<lgbits_) - 1;
}

/*------------------------------------------------------------------*/

// return smallest set index > a

int BitVector::
getNextSet (int a)
{
  if (a > maxn_)
    return -1;
  
  if (a == -1)
  {
    if (test(++a)) return a;
  }
  
  int index = (a >> lgbits_);
  unsigned int offset = a - (index << lgbits_) + 1;
  bvWordType * curWord = bvwords_ + index;
  int nwords = (maxn_>>lgbits_) + 1;
  
  do 
  {
    if (*curWord != 0)
    {
      
      while (offset < bitsPerWord_)
      {
        if ( *curWord & (1 << offset))
          return ( (index << lgbits_) + offset);
        offset++;
      }
    }
    
    offset = 0;
    curWord++;
    
  } while (++index < nwords);
  
  return -1;
  
}

int BitVector::
getPrevSet (int a)
{
  if(a <= 0)
    return -1;
  if(a > maxn_)
  {
    if(test(a = maxn_))
      return a;
  }

  int index = (a >> lgbits_);
  int offset = a - (index << lgbits_) - 1;
  bvWordType * curWord = bvwords_ + index;
  do {
    if(*curWord != 0)
    {
      while(offset >= 0)
      {
        if (*curWord & (1 << offset))
          return  ((index << lgbits_) + offset);
        offset--;
      }
    }
    offset = modMask_;
    curWord--;
  } while (--index >= 0);
  return -1;
}


//-----------------------------------------------------------
// return smallest clear index > a
int
BitVector::getNextClear (int a)
{
  if (a > maxn_)
    return -1;
  
  if (a == -1)
  {
    if (!test(++a)) return a;
  }
  
  int index = (a >> lgbits_);
  unsigned int offset = a - (index << lgbits_) + 1;
  bvWordType * curWord = bvwords_ + index;
  int nwords = (maxn_>>lgbits_) + 1;
  
  bvWordType allOne = (bvWordType) -1;
  bvWordType revWord;
  do 
  {
    if (*curWord != allOne)
    {	
      revWord = (*curWord ^ allOne);
      while (offset < bitsPerWord_ )
      {
        if (revWord & (1 << offset))
          return ( (index << lgbits_) + offset);
        offset++;
      }
      
    }
    
    offset = 0;
    curWord++;
    
  } while (++index < nwords);
  
  return -1;
  
}

int BitVector::
getPrevClear (int a)
{
  if(a <= 0)
    return -1;
  if(a >= maxn_+1)
  {
    if(a > maxn_+1)
      return --a;
    if(!test(--a))
      return a;
  }

  int index = (a >> lgbits_);
  int offset = a - (index << lgbits_) - 1;
  bvWordType * curWord = bvwords_ + index;
  bvWordType allOne = (bvWordType) -1;
  bvWordType revWord;
  do {
    if(*curWord != allOne)
    {
      revWord = (*curWord ^ allOne);
      while(offset >= 0)
      {
        if (revWord & (1 << offset))
          return  ((index << lgbits_) + offset);
        offset--;
      }
    }
    offset = modMask_;
    curWord--;
  } while (--index >= 0);
  return -1;
}
/*---------------------------------------------------------------------------*/

void BitVector::
adjustTo_ (BitVector& other, 
           unsigned int& nwords, 
           unsigned int& othernwords)
{
  nwords = (maxn_ >> lgbits_) + 1;
  othernwords = (other.maxn_ >> lgbits_) + 1;
  if (othernwords > nwords)
  {
    bvwords_ = (bvWordType *) 
      realloc ((char *) bvwords_, othernwords  * sizeof (bvWordType));
    BZERO (&bvwords_[nwords], bvWordType, othernwords - nwords);
    maxn_ = (othernwords << lgbits_) - 1;
    nwords = othernwords;
  }
}


/*--------------------------------------------------------------------------*/

BitVector &BitVector::
reverse ()
{
  numSet_ = -1;
  unsigned int nwords = (maxn_ >>lgbits_) + 1;
  for(unsigned int i=0; i<nwords; i++)
     bvwords_[i] = ~bvwords_[i];

  return *this;
}

/*---------------------------------------------------------------------------*/

BitVector& BitVector::
and_op (BitVector& other)
{
  unsigned int i;
  unsigned int nwords = (maxn_ >> lgbits_) + 1;
  unsigned int othernwords = (other.maxn_ >> lgbits_) + 1;
  unsigned int minwords = (nwords < othernwords)? nwords : othernwords;
  
  numSet_ = -1;
  for(i=0; i<minwords; i++)
  {
     bvwords_[i] &= other.bvwords_[i];
  }

  if (minwords < nwords)
    BZERO (&bvwords_[minwords], bvWordType, nwords - minwords);

  return *this;
}

/*---------------------------------------------------------------------------*/

BitVector& BitVector::
or_op (BitVector& other)
{
  unsigned int i;
  unsigned int nwords;
  unsigned int othernwords;
  adjustTo_ (other, nwords, othernwords);

  numSet_ = -1;
  for(i=0; i<othernwords; i++)
  {
     bvwords_[i] |= other.bvwords_[i];
  }
  
  return *this;
}

/*------------------------------------------------------------------*/

BitVector& BitVector::
xor_op (BitVector& other)
{
  unsigned int i;
  unsigned int nwords;
  unsigned int othernwords;
  adjustTo_ (other, nwords, othernwords);

  numSet_ = -1;
  for(i=0; i<othernwords; i++)
  {
     bvwords_[i] ^= other.bvwords_[i];
  }
  
  return *this;
}

/*---------------------------------------------------------------------------*/

BitVector& BitVector::
sub_op (BitVector& other)
{
  unsigned int i;
  unsigned int nwords = (maxn_ >> lgbits_) + 1;
  unsigned int othernwords = (other.maxn_ >> lgbits_) + 1;
  unsigned int minwords = (nwords < othernwords)? nwords : othernwords;
  
  numSet_ = -1;
  for(i=0; i<minwords; i++)
  {
     bvwords_[i] &= (~other.bvwords_[i]);
  }

  return *this;
}

/*---------------------------------------------------------------------------*/

BitVector& BitVector::
neg_op (void)
{
  unsigned int i;
  unsigned int nwords = (maxn_ >> lgbits_) + 1;
  numSet_ = -1;
  for(i=0; i<nwords; i++)
  {
    //     bvwords_[i] ^= (bvWordType) minusOne;
     bvwords_[i] = ~bvwords_[i];
  }

  return *this;
}

/*---------------------------------------------------------------------------*/

// BIG NOTE:  This only works if mapping is "monotonically decreasing".
// It needs to be done differently for other types of mapping.
void BitVector::
remap (IMap& map)
{
  int t2;
  for (int i = 0; i < map.size(); i++)
	{
    if (!map[i])
	{
		clear (i);
		continue;
	}
    t2 = map[i];
    if (test(i))
    {
      clear(i); 
      set (t2);
    }
	}
}

/*---------------------------------------------------------------------------*/

