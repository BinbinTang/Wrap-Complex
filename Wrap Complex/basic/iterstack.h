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

#ifndef __RGI_ITERSTACK_H__
#define __RGI_ITERSTACK_H__

#include <basic/rarray.h>

/**********************************************************************/

/*
 * Class IterStack -- A Resizable Stack
 *     This class will provide a stack object which automatically 
 *     resizes to store new elements.  In addition to the normal stack
 *     operations, the user can also index into the stack, and iterate
 *     over the elements of the stack.  
 *
 *     NOTE:  The objects are stored in locations 1 .. size().
 *
 *     The following options can be set:
 *
 *     Initial size:  the initial size of the stack (optional)
 *
 */

template <class T, class ObjSpec>
class IBaseStackIter;

template<class T, class ObjSpec>
class IterBaseStack
{
public:
  // Constructors

  friend class IBaseStackIter<T, ObjSpec>;

  IterBaseStack (int size = 20);

  // Destructor, Copy constructor and Assignment operator

  //  ~IterStack (void)
  IterBaseStack (IterBaseStack<T, ObjSpec> &other);
  const IterBaseStack<T, ObjSpec> &operator= (const IterBaseStack<T, ObjSpec> &other);

  // Basic stack operations
  inline int size (void) const;
  int reduceSizeBy (int n);
  inline T * getData (void);
  inline void push (const T &newElt);
  inline T pop (void);
  inline T &top (void);
  inline const T &top (void) const;
  inline int empty (void);
  void reset (void);

  // An indexing operator
  inline T &operator[] (int index);
  inline const T &operator[] (int index) const;

  // An iterator function and iteration operators for the stack
  void iter(void (* func) (T &elt));

  // Print operation
// @DT  friend ostream& operator<<(ostream& s, IterBaseStack<T, ObjSpec>& is);

  void sort ( int (*compFun) (const void *, const void *));
  void removeDuplicates (void);
  void resize (int newSize);
  Bool exist (const T &elt);
protected:

  RBaseArray<T, ObjSpec> arr_;        // An array of elements
  int numElts_;         // The number of elements in the stack
  static int compare_(const void * elt1, const void * elt2);
};

template<class T>
class IterStack : public IterBaseStack<T, DataContainer>
{
public:
  IterStack (int size = 20) : IterBaseStack<T, DataContainer>(size) { }
};

template<class T>
class IterObjStack : public IterBaseStack<T, ObjectContainer>
{
public:
  IterObjStack (int size = 20) : IterBaseStack<T, ObjectContainer>(size) { }
};

/****************************************************************************/

/*
 * Class IStackIter
 *
 * This class is an external iterator for an IterStack.  The method
 * first() is used to initialize the iterations, with next() continuing
 * to the next element.  If no next element exists, isDone() is true.
 * curItem() and curIndex() are used to access information about the
 * current item.
 *
 */

template <class T, class ObjSpec>
class IBaseStackIter
{
public:
  IBaseStackIter (IterBaseStack<T, ObjSpec>& is);

  // Iteration operations

  inline void first();
  inline void next();
  inline int isDone();     // This is true when the current element is not valid
  inline T &curItem();
  inline int curIndex();

private:
  IterBaseStack<T, ObjSpec>& is_;
  int current_;
};

template <class T>
class IStackIter : public IBaseStackIter<T, DataContainer>
{
public:
  IStackIter (IterStack<T>& is) : 
    IBaseStackIter<T, DataContainer>(is) { }
};

template <class T>
class IObjStackIter : public IBaseStackIter<T, ObjectContainer>
{
public:
  IObjStackIter (IterObjStack<T>& is) : 
    IBaseStackIter<T, ObjectContainer>(is) { }
};

#include <basic/iterstack.hpp>

#endif // __RGI_ITERSTACK_H__