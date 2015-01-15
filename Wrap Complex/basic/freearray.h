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

#ifndef __RGI_FREEARRAY_H__
#define __RGI_FREEARRAY_H__

#include <basic/rarray.h>
#include <basic/bitvector.h>



/****************************************************************************/

/*
 * Class FreeArray -- An implementation of a FreeArray that is fairly
 *         space efficient.  This implementation makes the following
 *         assumptions about the parameterized class T.
 *
 *             An object of type T cannot have any external objects
 *             that it is responsible for.  In other words, the copy
 *             constructor and assignment operators should essentially
 *             do bitwise copying.  For an explanation of this requirement,
 *             see the newLoc method below.
 */

template <class T>
class FArrayIter;

template <class T>
class FreeArray
{
public:
  FreeArray (int size = 20);

  // Destructor, Copy Constructor and Assignment Operator

  ~FreeArray (void);
  FreeArray (const FreeArray<T>& other);
  const FreeArray<T>& operator= (const FreeArray<T>& other);

  // Interface for accessing elements of the array

  // NOTE:  This class uses the array itself to store a freelist, overwriting
  //        the data with integer indices.  Therefore, if there are any
  //        pointer variables for which the object is owned by this object,
  //        then these pointers will be overwritten, and the memory will be
  //        lost.  Also, during the packing phase, if the assignment operator
  //        assumes that any memory has been allocated, this will cause
  //        problems.
  //
  //        One solution to this:  Use pointers instead of actual
  //        objects.

  inline T& operator[] (int index);    // checks for deletion implicitly
                                          // through bit vector

  inline int newLoc (void);               // returns a new location
  inline int addElt (const T& elem);
  inline void delElt  (int index);        // deletes element index
  inline Bool isValidIndex (int index);   // returns TRUE iff index is valid
  IMap *pack (void);                      // packs the array
  void resize (int newSize);              // provides storage of newSize directly
  // Size accessor functions for maximum size and number of elements used.

  inline int maxSize (void);
  inline int numUsed (void);
  inline int maxUsedIndex (void);
  void reset (void);

  // An iterator function for the stack
  void iter(void (* func) (T &elt));

  friend class FArrayIter<T>;
  FArrayIter<T> *newIter (void);

  // Print operation
  // @DT friend ostream& operator<<(ostream& s, FreeArray<T>& fa);

  BitVector& getBitVector (void) { return exists_; }
  void resetReusable (void) { nextReusable_ = 0; }
  T * getData (void) { return arr_.getData(); }

protected:
  int nextReusable_;              // Next reusable element
  RArray<T> arr_;                 // The elements themselves
  BitVector exists_;              // The elements that are currently in use
};

/****************************************************************************/

/*
 * Class FArrayIter
 *
 * This class is an external iterator for an FreeArray.  The method
 * first() is used to initialize the iterations, with next() continuing
 * to the next element.  If no next element exists, isDone() is true.
 * curItem() and curIndex() are used to access information about the
 * current item.
 *
 */

template <class T>
class FArrayIter
{
public:
  FArrayIter (FreeArray<T>& fa);

  // Iteration operations

  inline void first();
  inline void next();
  inline int isDone();     // This is true when the current element is not valid
  inline T &curItem();
  inline int curIndex();

private:
  FreeArray<T>& fa_;
  BitVectorIter iter_;
};


#include <basic/freearray.hpp>
/****************************************************************************/
#endif // __RGI_FREEARRAY_H__