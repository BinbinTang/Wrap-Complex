/*
 * rarray.h -- Classes for resizable arrays
 *
 * author -- Mike Facello
 *
 * date -- 10/8/96
 */

#ifndef __RGI_FARRAY_H__
#define __RGI_FARRAY_H__

#include <iostream>
#include "basic/basic.h"
#include "basic/bitvector.h"


/**********************************************************************/

/*
 * Class FBaseArray -- A Fixed Array
 *     This class defines an array object which automatically 
 *     resizes to store any accessed indices.  There are several 
 *     options that can be set at creation time:
 *
 *     Size:  the size of the array (optional)
 *
 */

class FDataContainer
{
public:
  enum { IS_OBJECT = 0 };
};

class FObjectContainer
{
public:
  enum { IS_OBJECT = 1 };
};


template<class T, class ObjSpec>
class FBaseArray 
{
public:
  // Some constants

  // Constructors
  FBaseArray (int size);

  // Destructor, Copy constructor and Assignment operator
  ~FBaseArray (void);
  FBaseArray (FBaseArray<T,ObjSpec> const &other);
  FBaseArray<T,ObjSpec> const &operator= (FBaseArray<T,ObjSpec> const &other);

  // Index operator -- This checks for index >= 0
  inline T &operator[] (int index);
  inline T &operator[] (int index) const;

  // The size of the array (this is only important for statistical purposes)
  inline int size(void);
  void resize (int newSize);

protected:
  int n_;                          // current size of array

  T *data_;                        // the array itself
};

// can contains data or objects without virtual pointer
template<class T>
class FixedArray : public FBaseArray<T, FDataContainer>
{
public:
  FixedArray (int size = 1) : FBaseArray<T, FDataContainer> (size) { }
  void reset (void);
};

// can contain data with virtual pointer
template<class T>
class FixedObjArray : public FBaseArray<T, FObjectContainer>
{
public:
  FixedObjArray (int size = 1) : FBaseArray<T, FObjectContainer> (size) { }
};

#include "basic/farray.hpp"

#endif /* __FARRAY_H__ */

