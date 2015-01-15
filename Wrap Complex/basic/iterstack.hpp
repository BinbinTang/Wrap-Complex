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


template <class T, class ObjSpec>
void IterBaseStack<T, ObjSpec>::
sort (int (*compFun) (const void *, const void *))
{
  arr_.sort (compFun, numElts_);
}

/**************************************************************************/

template <class T, class ObjSpec>
Bool IterBaseStack<T, ObjSpec>::
exist(const T &elt)
{
	for(int i=1;i<=numElts_;i++)
		if(elt==arr_[i])
			return TRUE;
	return FALSE;
}

/**************************************************************************/

template <class T, class ObjSpec>
int IterBaseStack<T, ObjSpec>::
compare_ (const void *elt1, const void *elt2)
{
  unsigned char * c1 = (unsigned char *) elt1;
  unsigned char * c2 = (unsigned char *) elt2;
  for(size_t i=0; i<sizeof(T); i++)
  {
    if(c1[i] != c2[i]) return (c1[i] - c2[i]); 
  }
  return 0;
}

/**************************************************************************/

template <class T, class ObjSpec>
void IterBaseStack<T, ObjSpec>::
removeDuplicates (void)
{
  if(empty()) return;

  sort(compare_);

  // count new number of elements
  register int i;

  T * newData = NULL;

  if (ObjSpec::IS_OBJECT)
  {
    int newNumElts = 1;
    for(i=2; i<= numElts_; i++)
    {
      if (arr_[i] != arr_[i-1]) newNumElts++;
    }

    newData = new T[newNumElts+1];
    newData[1] = arr_[1]; 
    
    register int j = 1;
    for(i=2; i<= numElts_; i++)
    {
      if (arr_[i] != arr_[i-1]) newData[++j] = arr_[i];
    }
    
    Assert(j == newNumElts);
    
    // update structure
    numElts_ = newNumElts;
    arr_.setData(newData, newNumElts+1);
  }
  else
  {
    register int j = 1;
    for(i=2; i<= numElts_; i++)
    {
      if(i != j && arr_[i] != arr_[j]) arr_[++j] = arr_[i];
    }
    numElts_ = j;
    arr_.resize(numElts_+1);
  }
}

/**************************************************************************/

/* Methods for Class IterStack */

template <class T, class ObjSpec>
IterBaseStack<T, ObjSpec>::
IterBaseStack (int size) 
  : arr_ (size),
    numElts_(0)
{ 
  
}


template <class T, class ObjSpec>
IterBaseStack<T, ObjSpec>::IterBaseStack (IterBaseStack<T, ObjSpec> &other)
            : arr_ (other.size()),
              numElts_ (0)
{
  for(int i=1; i<=other.size(); i++)
    push(other[i]);
}

template <class T, class ObjSpec>
const IterBaseStack<T, ObjSpec> &IterBaseStack<T, ObjSpec>::
operator= (const IterBaseStack<T, ObjSpec> &other)
{
  numElts_ = 0;
  resize(other.size());
  for(int i=1; i<=other.size(); i++)
    push(other[i]);
  return *this;
}

template <class T, class ObjSpec>
int IterBaseStack<T, ObjSpec>::
size (void) const
{ 
  return numElts_; 
} 

template <class T, class ObjSpec>
int IterBaseStack<T, ObjSpec>::
reduceSizeBy (int n)
{
  numElts_ -= n;
  if(numElts_ < 0) numElts_ = 0;
  return numElts_; 
}

template <class T, class ObjSpec>
T * IterBaseStack<T, ObjSpec>::
getData (void)
{
  return arr_.getData();
}


template <class T, class ObjSpec>
void IterBaseStack<T, ObjSpec>::
push (const T &newElt)
{
  arr_[++numElts_] = newElt;
}

template <class T, class ObjSpec>
T IterBaseStack<T, ObjSpec>::
pop (void)
{
  if (numElts_ == 0)
    {
		  Assert(0);
      exit(1);
    }
  return arr_[numElts_--];
}

template <class T, class ObjSpec>
T &IterBaseStack<T, ObjSpec>::
top (void)
{
  return arr_[numElts_];
}


template <class T, class ObjSpec>
const T &IterBaseStack<T, ObjSpec>::
top (void) const
{
  return arr_[numElts_];
}


template <class T, class ObjSpec>
int IterBaseStack<T, ObjSpec>::
empty (void)  
{ 
  return (numElts_ == 0); 
}


template <class T, class ObjSpec>
void IterBaseStack<T, ObjSpec>::
reset (void)  
{ 
  numElts_ = 0; 
}

template <class T, class ObjSpec>
T &IterBaseStack<T, ObjSpec>::
operator[] (int index)
{
  Assert ((index >= 1) && (index <= numElts_));
  return arr_[index];
}

template <class T, class ObjSpec>
const T &IterBaseStack<T, ObjSpec>::
operator[] (int index) const
{
  Assert ((index >= 1) && (index <= numElts_));
  return arr_[index];
}

template <class T, class ObjSpec>
void IterBaseStack<T, ObjSpec>::
iter(void (* func) (T &elt))
{
  for (register int i = 1; i <= numElts_; i++)
    func (arr_[i]);
}

template <class T, class ObjSpec>
void IterBaseStack<T, ObjSpec>::
resize (int newSize) 
{
  arr_.resize (newSize + 1); // start from 1
}


template <class T, class ObjSpec>
ostream&
operator<< (ostream& s, const IterBaseStack<T,ObjSpec>& is)
{
  s << "\nIterBaseStack with " << is.size() << " elements.\n\n";
  for (register int i = 1; i <= is.size(); i++)
    s << is[i] << endl;
  s << endl << endl;
  return s;
}

/* Methods for Class IStackIter */

template <class T, class ObjSpec>
IBaseStackIter<T, ObjSpec>::
IBaseStackIter (IterBaseStack<T, ObjSpec>& is)
  : is_ (is),
    current_ (0)
{ 
  
}

template <class T, class ObjSpec>
void IBaseStackIter<T, ObjSpec>::
first (void)
{
  current_ = 1;
}

template <class T, class ObjSpec>
void IBaseStackIter<T, ObjSpec>::
next (void)
{
  current_++;
}

template <class T, class ObjSpec>
int IBaseStackIter<T, ObjSpec>::
isDone (void)
{
  return (current_ > is_.numElts_);
}

template <class T, class ObjSpec>
T &IBaseStackIter<T, ObjSpec>::
curItem (void)
{
  return is_[current_];
}

template <class T, class ObjSpec>
int IBaseStackIter<T, ObjSpec>::
curIndex (void)
{
  return current_;
}
