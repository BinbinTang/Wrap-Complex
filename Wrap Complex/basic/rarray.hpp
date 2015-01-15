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

template <class T, class ObjSpec>
void RBaseArray<T, ObjSpec>::
sort (int (*compFun) (const void *, const void *), int numElts)
{
 if (numElts > n_) numElts = n_;
 if (numElts < 0) numElts = 0;
 qsort(&data_[1], numElts, sizeof(T), compFun);
}

//-----------------------------------------------------------------------------

template <class T, class ObjSpec>
RBaseArray<T, ObjSpec>::
RBaseArray (int size)
{ 
  n_ = size + 1;
  if (ObjSpec::IS_OBJECT)
    data_ = new T[n_];
  else
  {
    data_ = (T *) malloc(sizeof(T)*n_);
    BZERO (data_, T, n_);
  }
}


template <class T, class ObjSpec>
RBaseArray<T, ObjSpec>::
~RBaseArray (void)
{ 
  if (ObjSpec::IS_OBJECT)
    delete [] data_;
  else
    free (data_);
}

template <class T, class ObjSpec>
RBaseArray<T, ObjSpec>::
RBaseArray (RBaseArray<T, ObjSpec> const &other)
{
  n_ = other.size();
  T * data = other.getData();
  if (ObjSpec::IS_OBJECT)
  {
    data_ = new T [n_];
    for (register int i = 0; i < n_; i++)
      data_[i] = data_[i];
  }
  else
  {
    int sz = sizeof(T)*n_;
    data_ = (T *) malloc(sz);
    BCOPY(data, data_, sz);
  }
}

template <class T, class ObjSpec>
const RBaseArray<T, ObjSpec> &RBaseArray<T, ObjSpec>::
operator= (const RBaseArray<T, ObjSpec> &other)
{
  if (this == &other)
    return *this;

  n_ =  other.size();
  T * data = other.getData();
  if (ObjSpec::IS_OBJECT)
  {
    delete [] data_;
    data_ = new T[n_];
    for (register int i = 0; i < n_; i++)
	    data_[i] = data[i];
  }
  else
  {
    free (data_);
    int sz = sizeof(T)*n_;
    data_ = (T *) malloc(sz);
    BCOPY(data, data_, sz);
  }
  return (*this);
}

/*----------------------------------------------------------------*/

template <class T, class ObjSpec>
void RBaseArray<T, ObjSpec>::
setData (T * newData, int n)
{
  if (ObjSpec::IS_OBJECT)
  {
    delete [] data_;
  }
  else
  {
    if (data_) free (data_);
  }

  data_ = newData;
  n_ = n;
};


/*
 * RArray::operator[] -- returns a reference to the appropriate array
 *                       object, also checking for array bound violations
 */
template <class T, class ObjSpec>
T &RBaseArray<T, ObjSpec>::
operator[] (int index)
{
  assert (index >= 0);

  if (index >= n_)
  {
	  int newN = n_;
	  while ((newN - 1) < index)
	    newN *= scaleFactor_;
	  resize(newN);
  }

  return data_[index];
}


template <class T, class ObjSpec>
const T &RBaseArray<T, ObjSpec>::operator[] (int index) const
{
  Assert (index >= 0 && index < n_ );
 
  return data_[index];
}

/*
 * RArray::resize(size) -- resize the array to contain "size" elements of
 *                         "T".  This also zeros out the new elements of
 *                         the array if the AutoClear option is chosen.  
 */
template <class T, class ObjSpec>
void RBaseArray<T, ObjSpec>::
resize (int size)
{

  if (ObjSpec::IS_OBJECT)
  {
    T* newdata = new T[size];
    for (int i=0; i<n_; i++)
	    newdata[i] = data_[i];
    delete [] data_;
    data_ = newdata;
  }
  else
  {
    data_ = (T *) realloc(data_, sizeof(T)*size);
    if (size > n_)
    {
      BZERO((T *)(data_ + n_), T, size - n_);
    }
  }

  n_ = size;
}

template <class T>
void RArray<T>::
reset (void)
{
  BZERO(data_, T, n_);
}

