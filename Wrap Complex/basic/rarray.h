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

#ifndef __RGI_RARRAY_H__
#define __RGI_RARRAY_H__

#include <iostream>
#include <basic/basic.h>

class DataContainer
{
public:
  enum { IS_OBJECT = 0 };
};

class ObjectContainer
{
public:
  enum { IS_OBJECT = 1 };
};


template<class T, class ObjSpec>
class RBaseArray
{
enum { scaleFactor_ = 2 }; 
public:
  // Some constants

  // Constructors
  RBaseArray (int size);
  RBaseArray (RBaseArray<T, ObjSpec> const &other);
  // Destructor, Copy constructor and Assignment operator
  ~RBaseArray (void);
  const RBaseArray<T, ObjSpec> &operator= (const RBaseArray<T, ObjSpec> &other);

  // Index operator -- This checks for index >= 0
  inline T &operator[] (int index);
  inline const T &operator[] (int index) const;

  int size (void) const { return n_; }
  T * getData (void) const { return data_; }
  void setData (T * newData, int n);
  // Resize the array
  void resize (int newSize);
  void sort ( int (*compFun) (const void *, const void *), int numElts);
protected:
  int n_;                          // current size of array
  T * data_;                        // the array itself
};

// can contains data or objects without virtual pointer
template<class T>
class RArray : public RBaseArray<T, DataContainer>
{
public:
  RArray (int size = 1) : RBaseArray<T, DataContainer> (size) { }
  void reset (void);
};

// can contain data with virtual pointer
template<class T>
class RObjArray : public RBaseArray<T, ObjectContainer>
{
public:
  RObjArray (int size = 1) : RBaseArray<T, ObjectContainer> (size) { }
};

/* Now, include the implementations of all these functions... */

#include <basic/rarray.hpp>

#endif /* __RARRAY_H__ */

