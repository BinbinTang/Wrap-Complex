//
// Copyright 1996-99. Raindrop Geomagic, Inc.
// All Rights Reserved.
//
// This is unpublished proprietary source code of Raindrop Geomagic, Inc.;
// the contents of this file may not be disclosed to third parties, copied
// or duplicated in any form, in whole or in part, without the prior written
// permission of Raindrop Geomagic, Inc.
//
// This copyright notices may not be removed from this file at any time.
//
// RESTRICTED RIGHTS LEGEND:
// Use, duplication or disclosure by the Government is subject to restrictions
// as set forth in subdivision (c)(1)(ii) of the Rights in Technical Data
// and Computer Software clause at DFARS 252.227-7013, and/or in similar or
// successor clauses in the FAR, DOD or NASA FAR Supplement. Unpublished-
// rights reserved under the Copyright Laws of the United States.
//
//                 Raindrop Geomagic, Inc
//                 206 N. Randolph Street, Suite 520
//                 Champaign IL 61820-3898
//                 Phone: 217-239-2551
//                 Fax:   217-239-2556
//                 Email: inquiry@geomagic.com
//
//

/**************************************************************************/

/* Methods for Class FreeArray */

// BIG NOTE:  This class *cannot* be used with any object that contains
//            pointers, except if you don't want to pack!!

template <class T>
FreeArray<T>::
FreeArray (int size)
  : nextReusable_ (0),
    arr_ (size),
    exists_ (size+1)
{ 
  Assert (sizeof(T) >= sizeof(int));
}


template <class T>
FreeArray<T>::
~FreeArray (void) 
{ 

}


template <class T>
FreeArray<T>::
FreeArray (const FreeArray<T>& other)
: nextReusable_(other.nextReusable_),
  arr_(other.arr_),
  exists_(other.exists_)
{
  
}


template <class T>
const FreeArray<T> &FreeArray<T>::
operator= (const FreeArray<T> &other)
{
  if (this != &other)
    {
      nextReusable_ = other.nextReusable_;
      arr_ = other.arr_;   // Does this work???
      exists_ = other.exists_;
    }
  return (*this);
}


template <class T>
T& FreeArray<T>::
operator[] (int index)
{
  Assert ((index >= 0) && (exists_.test(index)));

  /*
  if ((index < 0) || !exists_->test(index))
    {
      basic_error ("Error: FreeArray element %d does not exist!\n", index);
      Assert_always (FALSE);
    }      
    */

  return arr_[index];
}



// newLoc(void)
//
//    This method will return the index of an empty location in the
//    array.  BIG NOTE:  no assumptions should be made about the state 
//    of the object in this location.  All variables should be initialized.
//    

template <class T>
int FreeArray<T>::
newLoc (void)
{
  int returnVal = nextReusable_;
  if (nextReusable_ != 0)
    {
      int *ptr = (int *) (& arr_[nextReusable_]);
      returnVal = nextReusable_;
      nextReusable_ = *ptr;
    }
  else
    {
      // NOTE:  This assumes correct implementation...  If there are no
      // reusable elements, then elements 1..numUsed() must all be full.
 
      returnVal = numUsed() + 1;
    }

  Assert_always (!exists_.test (returnVal));
  exists_.set (returnVal);
  return returnVal;
}


// delElt (index)
//
//    This method will mark an array element as unused and put it on
//    the free list.  NOTE:  Prior to deleting an element, any dynamic
//    memory should be deallocated...  otherwise, it may be lost.
//

template <class T>
void FreeArray<T>::
delElt (int index)
{
  int *ptr;

  Assert_always ((index > 0) and exists_.test (index));
  ptr = (int *) &(arr_[index]);
  *ptr = nextReusable_;
  nextReusable_ = index;
  exists_.clear (index);
}


template <class T>
Bool FreeArray<T>::
isValidIndex (int index)
{
  return (exists_.test (index));
}

template <class T>
int FreeArray<T>::
addElt(const T& elem)
{
  int i = newLoc();
  arr_[i] = elem;
  return i;
}


//
// pack () -- This function will pack the array, removing empty holes.
//

template <class T>
IMap *FreeArray<T>::
pack (void)
{
  IMap *map = new IMap(maxSize());
  
  int cur = 0;

  for (register int i = 1; i <= maxSize(); i++)
    if (exists_.test(i))
      {
	(*map)[i] = ++cur;
	arr_[cur] = arr_[i];
	exists_.clear(i);
	exists_.set  (cur);
      }

  Assert_always (cur == numUsed ());
  nextReusable_ = 0;

  return map;
}

#include <basic/comment.h>

template <class T>
void FreeArray<T>::
resize (int newSize)
{
  arr_.resize(newSize + 1); // start from 1
}

template <class T>
int FreeArray<T>::
maxSize (void)
{
  return arr_.size();
}

template <class T>
int FreeArray<T>::
numUsed (void)
{
  return exists_.getNumSet();
}

template <class T>
int FreeArray<T>::
maxUsedIndex (void)
{
  return exists_.maxSetIndex();
}

template <class T>
void FreeArray<T>::
reset (void)
{
  exists_.reset();
  nextReusable_ = 0;
}


template <class T>
void FreeArray<T>::
iter(void (* func) (T &elt))
{
  for (register int i = 1; i <= maxSize(); i++)
    if (exists_.test (i))
      func (arr_[i]);
}


template <class T>
FArrayIter<T> *FreeArray<T>::
newIter (void) 
{ 
  return new FArrayIter<T> (*this); 
}

template <class T>
ostream& 
operator<<(ostream& s, const FreeArray<T>& fa)
{
  s << "\nFreeArray with size " << fa.maxSize() << " and "
    << fa.numUsed () << " elements\n\n";
  for (register int i = 1; i <= fa.maxSize(); i++)
    if (fa.exists_.test (i))
      s << fa[i] << endl;
  s << endl;
  return s;
}


/**************************************************************************/

/* Methods for Class FArrayIter */

template <class T>
FArrayIter<T>::
FArrayIter (FreeArray<T>& fa)
  : fa_ (fa),
    iter_ (fa.getBitVector()) 
{ 
  
}


template <class T>
void FArrayIter<T>::
first (void)
{
  iter_.first();
}

template <class T>
void FArrayIter<T>::
next (void)
{
  iter_.next();
}

template <class T>
int FArrayIter<T>::
isDone (void)
{
  return iter_.isDone();
}

template <class T>
T &FArrayIter<T>::
curItem (void)
{
  return fa_.arr_[iter_.curItem()];
}

template <class T>
int FArrayIter<T>::
curIndex (void)
{
  return iter_.curItem();
}
