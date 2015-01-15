/*
 * simph.cpp -- C++ Implementation file for Simplex Hash Table Class
 *
 *
 * author -- Mike,Dmitry,Damrong
 *
 *               
 *
 * date -- 08/28/97
 *	-- 04/22/98
 */

#include <basic/binio.h>
#include <basic/comment.h>
#include <basic/convert.h>
#include <iostream>
#include <geometry/geomutil.h>
#include <geometry/simph.h>


/*----------------------------------------------------------------------------*/
/*--------------------- ABSTRACT CLASS SimpHTable ----------------------------*/
/*----------------------------------------------------------------------------*/

SimpHTable::
SimpHTable (Trist *tr, Dim d, int size, Bool isRandom)
:     magic_ (                     d+10000),
       size_ (basic_prime_successor (size)), 
    numElts_ (                           0),
#ifndef _DYNAMIC_
  keyLength_ (        (d+1) * sizeof (int)),
#else
  keyLength_ (                       (d+1)),
#endif
         tr_ (                          tr),
          d_ (                           d),
          a_ (         new int[keyLength_]),
   isRandom_ (                    isRandom)
{
  uhashNew_ ();
}

/*----------------------------------------------------------------------------*/

SimpHTable::
~SimpHTable (void)
{
  delete [] a_;
}

/*----------------------------------------------------------------------------*/

Bool SimpHTable::
binInit (BinInput & bi)
{
  if (!magic_.binInit (bi))
  {
    COMMENT("Incorrect magic number for " << d_ << "-SimpHTable.");
    return FALSE;
  }

  size_ = bi.readInt ();         // two duplicate copies for a historical reason
  size_ = bi.readInt ();
  hashReadA_ (bi, a_, keyLength_);
  return TRUE;
}


/*----------------------------------------------------------------------------*/

void SimpHTable::
binDump (BinOutput& bo)
{
  // file format : (magic, size_, size_, a_[.])
  magic_.binDump(bo);
  bo.writeInt(size_);		 // two duplicate copies for a historical reason
  bo.writeInt(size_);
  hashWriteA_ (bo, a_, keyLength_);
}

/*----------------------------------------------------------------------------*/

ostream&
operator<< (ostream& s, SimpHTable& simpHTable)
{
  int size = simpHTable.size ();
  VertArray vArray;
  
  for (register int i = 0; i <= size; i++)    // {F/R}SimpHTable {0/1}-based
    if (simpHTable.isValidIndex (i))          // so 0..size scan all
    {
	simpHTable.tr_->getVertices (simpHTable[i], simpHTable.d_, vArray);
     	s << i << " : " << vArray;
    }
  return s;
}

/*----------------------------------------------------------------------------*/

void SimpHTable::
uhashNew_ (void)
{
  int i;

  unsigned int max_a =
#ifndef _DYNAMIC_
    (MAXINT / (int) powerof2 (bitsof (Basic_byte))) / keyLength_;
#else
    (MAXINT / (int) powerof2 (18)         ) / keyLength_;
    // assume < 2^18 = 256K accumulated total vertices
#endif
  unsigned int ma = (size_ < (int)max_a)? (size_ - 1) : (max_a - 1);

  if (isRandom_)
  {
    for (i = 0; i< keyLength_; i++)
    {
      a_[i] = 1 + ((int) rand () % ma);
    }
  }
  else
  {
    int val = ((size_/5 + 2)*(size_/3 + 1))% (size_/2) + 1965;
    for (i = 0; i< keyLength_; i++)
    {
      a_[i] = 1 + ( val % ma);
      val += (size_/3+1) + (a_[i]*71) + 1;
    }
  }
}

static void swap4Byte (char *ptr, int count)
{
	while (count--)
	{
		char tmp0 = ptr[0];
		char tmp1 = ptr[1];
		ptr[0] = ptr[3];
		ptr[1] = ptr[2];
		ptr[2] = tmp1;
		ptr[3] = tmp0;
		ptr += 4;
	}
}

/*----------------------------------------------------------------------------*/

int SimpHTable::
uhash_ (VA_SimpKey& vaKey)
{
#ifndef _DYNAMIC_
  Basic_byte * v = (Basic_byte *) vaKey.getVertices ()->getArray();
#else
  int * v = vaKey.getVertices ()->getArray();
#endif

  int sum = 0;
  for (int i=0; i<keyLength_; i++)
  {
    sum += (a_[i]*v[i]) ;
  }

  return sum % size_;
}

/*----------------------------------------------------------------------------*/

void 
SimpHTable::
hashWriteA_ (BinOutput& bo, int *a, int keyLength)
{
  char *aXdr, *aTmp, *aChar;
//  int *aXdr;
//  unsigned char *aTmp, *aChar;
  int i, nofIntsInKey;
  unsigned j, k;


  Assert_always ((keyLength % sizeof (int)) == 0);

  aXdr = MALLOC (char, INT_SIZE * keyLength);
  aTmp = MALLOC (char, INT_SIZE * keyLength);
//  aXdr = MALLOC (int, keyLength);
//  aTmp = (unsigned char *) MALLOC (int, keyLength);

  nofIntsInKey = keyLength / sizeof (int);

//  encodeInteger<int> (aXdr, a, INT_SIZE, keyLength);
  encodeInteger(aXdr, a, INT_SIZE, keyLength);
//  xdrConvertToXdrInts (a, aXdr, keyLength);


  aChar = aXdr;
//  aChar = (unsigned char *) aXdr;

//  for (i = 0; i < nofIntsInKey; i++) {
//    for (j = 0; j < sizeof (int); j++)
//      for (k = 0; k < sizeof (int); k++) {
//        aTmp [i * sizeof (int) * sizeof (int) + j * sizeof (int) + k] =
//             aChar [i * sizeof (int) * sizeof (int) + k * sizeof (int) + j];
//      }
//  }

  for (i = 0; i < nofIntsInKey; i++) {
    for (j = 0; j < INT_SIZE; j++)
      for (k = 0; k < INT_SIZE; k++) {
        aTmp [i * INT_SIZE * INT_SIZE + j * INT_SIZE + k] =
             aChar [i * INT_SIZE * INT_SIZE + k * INT_SIZE + j];
      }
  }


  //xdrConvertToXdrUnsignedInts ((unsigned int *) aTmp,
  //                             aPortable, keyLength);
  // binfwrite (aPortable, keyLength, fp);
#if defined(LITTLE_ENDIAN)
  swap4Byte (aTmp, keyLength);
#endif
  bo.writeChars (aTmp, keyLength * INT_SIZE);
//  bo.writeUnsignedInts((unsigned int *) aTmp, keyLength);
  

  FREE (aTmp);
  FREE (aXdr);
}

/*----------------------------------------------------------------------------*/

void
SimpHTable::
hashReadA_ (BinInput& bi, int *a, int keyLength)
{
  char *aXdr, *aTmp, *aChar;
//  int *aXdr;
//  unsigned char *aTmp, *aChar;
  int i, nofIntsInKey;
  unsigned j, k;

  Assert_always ((keyLength % sizeof (int)) == 0);

  nofIntsInKey = keyLength / sizeof (int);

  aXdr = MALLOC (char, INT_SIZE * keyLength);
  aTmp = MALLOC (char, INT_SIZE * keyLength);
//  aXdr = MALLOC (int, keyLength);
//  aTmp = (unsigned char *) MALLOC (int, keyLength);

  bi.readChars(aTmp, INT_SIZE * keyLength);
#if defined(LITTLE_ENDIAN)
  swap4Byte (aTmp, keyLength);
#endif
  //  bi.readUnsignedInts((unsigned int *) aTmp, keyLength);

  aChar = aXdr;
//  aChar = (unsigned char *) aXdr;


//  for (i = 0; i < nofIntsInKey; i++) {
//    for (j = 0; j < sizeof (int); j++)
//      for (k = 0; k < sizeof (int); k++) {
//        aChar [i * sizeof (int) * sizeof (int) + j * sizeof (int) + k] =
//              aTmp [i * sizeof (int) * sizeof (int) + k * sizeof (int) + j];
//      }
//  }

  for (i = 0; i < nofIntsInKey; i++) {
    for (j = 0; j < INT_SIZE; j++)
      for (k = 0; k < INT_SIZE; k++) {
        aChar [i * INT_SIZE * INT_SIZE + j * INT_SIZE + k] =
              aTmp [i * INT_SIZE * INT_SIZE + k * INT_SIZE + j];
      }
  }


  decodeInteger(a, aXdr, INT_SIZE, keyLength);
//  xdrConvertFromXdrInts (aXdr, a, keyLength);
  FREE (aTmp);
  FREE (aXdr);
}

/*----------------------------------------------------------------------------*/
/*---------------------------- CLASS FSimpHTable -----------------------------*/
/*----------------------------------------------------------------------------*/

FSimpHTable::
FSimpHTable (Trist * tr, Dim d, int size, Bool isRandom)
  : SimpHTable  (tr, d, 2*size, isRandom),     //'double' size reduce collosion
          used_ (              size_ + 1),
         valid_ (              size_ + 1),
           arr_ (    new OrTri[size_ + 1])  
{
}

//----------------------------------------------------------------------------

FSimpHTable::
~FSimpHTable (void)
{
  delete [] arr_;
}

/*----------------------------------------------------------------------------*/

int FSimpHTable::
searchKey (VA_SimpKey& vaKey)
{
  int index = uhash_ (vaKey);
  int increment = 1 + (index % (size_ - 2));
  
  while (used_.test (index)) 
  {
    if (valid_.test(index))
    {
      EF_SimpKey key (arr_[index], d_, &vTest_, tr_);
      if (compare_ (vaKey, key))
        return index;
    }
    index = (index + increment) % size_;
  }

  return NIL;
}

/*----------------------------------------------------------------------------*/

int FSimpHTable::
insertKey (EF_SimpKey& efKey)
{ 
  VA_SimpKey vaKey(efKey.getVertices ());
  int index = uhash_ (vaKey);
  int increment = 1 + (index % (size_ - 2));

  while (valid_.test(index))
  {
    EF_SimpKey key(arr_[index], d_, &vTest_, tr_);
    if (compare_ (vaKey, key))
      return index;

    index = (index + increment) % size_;
  }

  used_.set (index);
  valid_.set (index);
  arr_[index] = efKey.getEF ();
  return index;
}

/*----------------------------------------------------------------------------*/

Bool FSimpHTable::
del (int ix)
{
  Bool res = valid_.test(ix);
  valid_.clear(ix);
  return res;
}

/*----------------------------------------------------------------------------*/

Bool FSimpHTable::
binInit (BinInput & bi)
{
  return SimpHTable::binInit (bi);
}

/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/*---------------------------- CLASS RSimpHTable -----------------------------*/
/*----------------------------------------------------------------------------*/


RSimpHTable::
RSimpHTable(Trist *tr, Dim d, int size, Bool isRandom,
            int maxProbeLen, float minEmptyRatio, float maxDelRatio)
:   SimpHTable  (tr, d, 2*size), // double expected size to reduce collosion
         magic_ (       d+2000), // different magic_ than SimpHTable
           arr_ (         size), // arr_ has the expected size
   maxProbeLen_ (  maxProbeLen),
 minEmptyRatio_ (minEmptyRatio),
   maxDelRatio_ (  maxDelRatio),
        numDel_ (            0),
      numEmpty_ (        size_),
   avgProbeLen_ (            0),
     numSearch_ (            0),
  enableResize_ (         TRUE),
  resizeCount1_ (            0),
  resizeCount2_ (            0)
{
	      ix_ =      new RArray <int> (size_);      // only ix_ need size_ = 2*size
  minEmpty_ = (int)(minEmptyRatio_ * size_);
	  maxDel_ = (int)(  maxDelRatio_ * size_);
  Assert_always(minEmpty_    > 1);
  Assert_always(maxProbeLen_ > 1);

  //------------------------------------------------------
  // initialize ix_ to (NIL,..,NIL)
  // (*ix_)[i]==NIL means the entry is empty.
  //------------------------------------------------------
  int i;
  for(i=0; i<ix_->size(); i++) (*ix_)[i]=NIL;

  COMMENT("maxProbeLen,minEmptyRatio,maxDelRatio" << "\t"
       <<  maxProbeLen_ <<","<< minEmptyRatio_ <<","<< maxDelRatio_);
}


/*----------------------------------------------------------------------------*/


int RSimpHTable::
searchKey(VA_SimpKey& vaKey)
{
  int       h2,v2 ;
  double    probeLen=1;
  VertArray tempV;

  probeIndex_ = uhash_(vaKey);
   freeIndex_ = -1;
	 
  while ((*ix_)[probeIndex_] != NIL)
  {
		if (DEL == (*ix_)[probeIndex_])
		{
		    freeIndex_ = probeIndex_;
		}
		else
		{
        Assert_always (arr_.isValidIndex((*ix_)[probeIndex_]));

        EF_SimpKey key (arr_[(*ix_)[probeIndex_]], d_, &tempV, tr_);
        if (compare_ (vaKey, key))
				{
            avgProbeLen_ = (avgProbeLen_ * numSearch_ + probeLen)
                          /(numSearch_+1);  numSearch_++;
            return (*ix_)[probeIndex_];				                        // FOUND !
				}
    }
    v2          = vaKey.getVertices()->getVertex (2);
    h2          = 1 + v2 % (size_-2);         // relatively prime to prime size_
    probeIndex_ = (probeIndex_ + h2) %size_;
    probeLen++;
  }

  if (freeIndex_ == -1)
	{
	    freeIndex_ = probeIndex_;
	}
  avgProbeLen_ = (avgProbeLen_ * numSearch_ + probeLen)
                 /(numSearch_+1);  numSearch_++;
  return NIL;                                                    // Not exists !
}


/*----------------------------------------------------------------------------*/


int RSimpHTable::
insertKey(EF_SimpKey& efKey)
{
  int s;

  if(numEmpty_<=minEmpty_)
  {                       			  // prevent lost of empty entry
      resizeCount1_++;
      resize_ (EXPAND*numElts_); 
  }

  if (NIL != (s = SimpHTable::searchKey (efKey))) 
  {
	return s;               			  // item already exists
  }

  numEmpty_--; 
   numElts_++; 

  s = arr_.newLoc ();

  arr_ [(*ix_) [freeIndex_] = s ] = efKey.getEF();

  return s;
}


/*----------------------------------------------------------------------------*/


Bool RSimpHTable::
del (int index)
{
  if (!isValidIndex(index))
  {
     return FALSE;
  }

  if (numDel_>maxDel_)
  {  
     resizeCount2_++;
     resize_ (REHASH*numElts_);
  }

  search (arr_ [index]);			              // set probeIndex_
 
  arr_.delElt ((*ix_) [probeIndex_]);

  (*ix_) [probeIndex_] = DEL;

  numElts_--; 
   numDel_++;

  return TRUE; 
}


/*----------------------------------------------------------------------------*/


Bool RSimpHTable::
binInit (BinInput& bi)
{
  // file format (magic, arr)

  minEmpty_ = (int)(minEmptyRatio_ * size_);
    maxDel_ = (int)(  maxDelRatio_ * size_);

  if (!magic_.binInit (bi))
  {
    COMMENT("Incorrect magic number for " << d_ << "-RSimpHTable.");
    return FALSE;
  }

  int result = arr_.binInit (bi);
  Assert_always (result);

  numElts_ = arr_.numUsed ();

  resize_ (REHASH * numElts_);

  return TRUE;
}


/*----------------------------------------------------------------------------*/


void RSimpHTable::
binDump (BinOutput& bo)
{
  // file format (magic, arr)

  Assert_always (numElts_ == arr_.numUsed ());

  magic_.binDump (bo);
    arr_.binDump (bo); 
}


/*----------------------------------------------------------------------------*/


void RSimpHTable::
resize_(int size)
{
  int i;

  delete [] a_; 
     a_  = new int [keyLength_];
  size_  = basic_prime_successor (size);

  Assert_always (size_ >= numElts_);                     

  uhashNew_ ();                               // new Hash Function for new size_

  for (i=0; i < size_; i++)
  {
    (*ix_) [i] = NIL;                                // init ix_ to (NIL,..,NIL)
  }

  //------------
  // rehash ix_ 
  //------------

  enableResize_ = FALSE;                  // prevent re-entrances from search ()

  int                 s;
  VertArray           v;
  FArrayIter <OrTri>  arrIt (arr_);
      
  for (arrIt.first (); !arrIt.isDone (); arrIt.next ())    
  {
    EF_SimpKey key (arrIt.curItem (), d_, &v, tr_);
    s = SimpHTable::searchKey (key);

    Assert_always (s == NIL);   

    (*ix_)[freeIndex_] = arrIt.curIndex ();       // freeIndex_ set in searchKey
  }

  enableResize_ = TRUE;                              // enable resize after this
  
      minEmpty_ = (int)(minEmptyRatio_ * size_);
        maxDel_ = (int)(  maxDelRatio_ * size_);
        numDel_ = 0;
      numEmpty_ = size_ - numElts_;
   avgProbeLen_ = 0.0;
     numSearch_ = 0.0;
}


/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*--------------- CLASS RSimpHTable:: PFOrTriA -------------------------------*/
/*-------------------- (Persistent FreeArray <OrTri> -------------------------*/
/*----------------------------------------------------------------------------*/


RSimpHTable::
PFOrTriA::
PFOrTriA(int size)
  : FreeArray<OrTri> (size)
{
}


/*----------------------------------------------------------------------------*/


Bool RSimpHTable::
PFOrTriA::
binInit (BinInput& bi)
  //
  // file format : (numUsed_,exists_,intRep(OrTri)[.])
  //
  // This function tightly depends on internal structure of FreeArray
  // especially the BitVector exists_ and RArray arr_
{
  int i, numRead, lastSet, result;
  
  int numUsed = bi.readInt ();                             // read numUsed_

  result = exists_.binInit (bi);			// read exists_->[]
  Assert_always (result);

  //------------------
  // consistency check
  //------------------
  Assert_always (!(exists_.test (0)));		// FreeArray is 1-based
						// BitVector is 0-based
  Assert_always (numUsed == exists_.getNumSet());

  //--------------
  // read arr_
  //--------------
        i = 0;
  numRead = 0;

  while (0 < (i = exists_.getNextSet (i))) 
  {
    arr_[lastSet = i].setIntRep (bi.readInt ());
    numRead ++;
  }

  Assert_always (numRead == numUsed); 

  //-----------------------------------------------------------
  //create Free List by 
  //       scanning through array from tail back to head.
  //       We can conventionally scan from head to tail, but
  //       reverse scan may hopefully give better distribution.
  //-----------------------------------------------------------
  int nextReusable_ =0;                        // nextReusable_==0 means
                                               // entry [1]..[numUsed_] are used
  for (i = lastSet - 1; i>0; i--)
  {
    if (!(exists_.test (i)))
    {
      int * ptr = (int*) & (arr_[i]);
          * ptr = nextReusable_;
      nextReusable_ = i;
    }
  }

  return TRUE;
}


/*----------------------------------------------------------------------------*/


void RSimpHTable::
PFOrTriA::
binDump (BinOutput& bo)
{
  // file format : (numUsed_, exists_, intRep(OrTri)[.])

  int      numWrite=0;
  FArrayIter <OrTri> fa(*this);

  int numUsed = exists_.getNumSet ();

  bo.writeInt (numUsed);                        // write numUsed
                                                 // nextReusable_ is not written
  exists_.binDump (bo);                         // write BitVector exists_->[]

  // write arr_

  for (fa.first (); !fa.isDone (); fa.next ())
  {
    bo.writeInt (fa.curItem ().getIntRep ());
    numWrite++;
  }   

  Assert_always (numWrite == numUsed);
}


