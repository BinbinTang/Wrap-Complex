#ifndef __RGI_DUMPABLE_H__
#define __RGI_DUMPABLE_H__

#include <basic/iterstack.h>
#include <basic/freearray.h>
#include <basic/callbackobject.h>
#include <basic/index.h>

//*************************************************************
// Dumpable versions of simple types
//*************************************************************

class IntDumpable
{
  int rep_;
public:
  IntDumpable (int rep = 0) : rep_(rep) { }
  operator int (void) { return rep_; }
  void binDump (BinOutput& bo) { bo.writeInt(rep_); }
  Bool binInit (BinInput& bi) { return rep_ = bi.readInt(), TRUE; }
};

class FloatDumpable
{
  float rep_;
public:
  FloatDumpable (float rep = 0.0f) : rep_(rep) { }
  operator float (void) { return rep_; }
  void binDump (BinOutput& bo) { bo.writeFloat(rep_); }
  Bool binInit (BinInput& bi) { return rep_ = bi.readFloat(), TRUE; }

};

class DoubleDumpable
{
  double rep_;
public:
  DoubleDumpable (double rep = 0.0) : rep_(rep) { }
  operator double (void) { return rep_; }
  void binDump (BinOutput& bo) { bo.writeDouble(rep_); }
  Bool binInit (BinInput& bi) { return rep_ = bi.readDouble(), TRUE; }
};

//*******************************************************************
// Dumpable arrays
//*******************************************************************


template<class T>
class RArrayDumpable : public RArray<T>, public ProgressCallbackObject
{
public:
  RArrayDumpable (int size = 1) : RArray<T>(size) { }
  RArrayDumpable (RArray<T>& other) : RArray<T>(other) { }
  Bool binInit (BinInput& bi);
  void binDump (BinOutput& bo);
};

template<class T>
class RObjArrayDumpable : public RObjArray<T>, public ProgressCallbackObject
{
public:
  RObjArrayDumpable (int size = 1) : RObjArray<T>(size) { }
  RObjArrayDumpable (RObjArray<T>& other) : RObjArray<T>(other) { }
  Bool binInit (BinInput& bi);
  void binDump (BinOutput& bo);
};

template<class T>
class IterStackDumpable : public IterStack<T>, public ProgressCallbackObject
{
public:
  IterStackDumpable (int size = 20) : IterStack<T>(size) { }
  IterStackDumpable (IterStack<T>& s) : IterStack<T>(s) { }
  Bool binInit (BinInput& bi);
  void binDump (BinOutput& bo);
};

template<class T>
class IterObjStackDumpable : public IterObjStack<T>, public ProgressCallbackObject
{
public:
  IterObjStackDumpable (int size = 20) : IterObjStack<T>(size) { }
  IterObjStackDumpable (IterObjStack<T>& s) : IterObjStack<T>(s) { }
  Bool binInit (BinInput& bi);
  void binDump (BinOutput& bo);
};

template<class T>
class FreeArrayDumpable : public FreeArray<T>, public ProgressCallbackObject
{
public:
  FreeArrayDumpable (int size = 20) : FreeArray<T> (size) { }
  FreeArrayDumpable (const FreeArray<T>& other) : FreeArray<T>(other) { }
  const FreeArrayDumpable<T>& operator= (const FreeArray<T>& other)
  { FreeArray<T>::operator=(other); return *this; }
  Bool binInit (BinInput& bi);
  void binDump (BinOutput& bo);
};

/****************************************************************************/

/*  Class IntRArray  */

/****************************************************************************/

class IntRArray : public RArray<int>
{
public:
  IntRArray (int size = 20) : RArray<int> (size) { }
  Bool binInit (BinInput& bi);
  void binDump (BinOutput& bo);
};

/****************************************************************************/

/*  Class DoubleRArray  */

/****************************************************************************/

class DoubleRArray : public RArray<double>
{
public:
  DoubleRArray (int size = 20) : RArray<double> (size) { }
  Bool binInit (BinInput& bi);
  void binDump (BinOutput& bo);
};

/****************************************************************************/

/*  Class DoubleRArray  */

/****************************************************************************/

class FloatRArray : public RArray<float>
{
public:
  FloatRArray (int size = 20) : RArray<float> (size) { }
  Bool binInit (BinInput& bi);
  void binDump (BinOutput& bo);
};



/****************************************************************************/

/*
 *  Class IMap -- mapping index class. Useful for many things.
 *  Initialized with all zeroes. Resizing is responsibility of
 *  external user.
 * 
 */

class IMap : public RArray<int>
{
public:
  IMap (unsigned int size = 20) : RArray<int>(size) { }
  ~IMap (void) { }
};

/************************************************************************
*
*
*     class IndexStack
*
*/

class IndexStack: public IterStack<Index>
{
public:
  IndexStack (int size = 10) : IterStack<Index> (size) { }
 ~IndexStack (void) { }
  Bool binInit (BinInput& bi);
  void binDump (BinOutput& bo);
};

class IxStackIter: public IStackIter<Index>
{
public:
  IxStackIter (IndexStack & ixStack) : IStackIter<Index> (ixStack) { }
 ~IxStackIter (void) { }
};

#include <basic/dumpable.hpp>

#endif // __DUMPABLE_H__