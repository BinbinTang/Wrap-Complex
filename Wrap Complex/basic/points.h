/*
 * points.h -- Class definitions for all point and point array classes.
 *
 * author -- Mike Facello
 *
 * date -- 10/7/96
 */

#ifndef __RGI_POINTS_H__
#define __RGI_POINTS_H__

#include <iostream>
#include <basic/basic.h>
#include <basic/binio.h>
#include <basic/dumpable.h>
#include <basic/callbackobject.h>
#include <basic/rgivector.h>
#include <basic/rgitypes.h>

/*  Number of coordinates per vertex, plus constants for accessing the
    correct elements of the arrays */

#define X  1
#define Y  2
#define Z  3
#define W  4

class VertexArray;

class CoordNumber
{
public:
	enum { NUM_COORDS = 3 };
};

class WeightedCoordNumber
{
public:
	enum { NUM_COORDS = 4 };
};


//--------------------------------------------------------------------------

//. CLASS Point
//-
//- This class stores a point consisting of four coordinates:  three
//- coordinates in normal three-dimensional space, and a fourth coordinate
//- adjusted for the weight of the point.

template <class T, class sz>
class Point
{
 typedef struct 
 {
	 T data[sz::NUM_COORDS];
 } pointArrayType;

public:
  Point (void);
  Point (Point<T, sz> const &other);

  ~Point (void);
  Point<T, sz> const &operator= (Point<T, sz> const &other);

  inline T &operator[] (int index);
  inline const T &operator[] (int index) const;
  inline int getNumCoords (void);
  inline T * getData (void);

#ifndef __GNUC__
  friend ostream& operator<< (ostream& s, Point<T, sz>& p);
#endif

protected:
  pointArrayType coords_;
};

//--------------------------------------------------------------------------

//. CLASS IntPoint
//-
//- A subclass of Point using integers for coordinates.

class IntPoint : public Point<int, WeightedCoordNumber>
{
public:
  IntPoint (void);
  IntPoint (BinInput& bi);
  void load(int x, int y, int z, int r = 0);

  void binDump(BinOutput& bo);

  friend ostream&  operator<< (ostream& s, const IntPoint& p);
};


//--------------------------------------------------------------------------

//. CLASS RealPoint
//-
//- A subclass of Point using integers for coordinates.  double is
//- used to specifiy which type of floating poing number should be used
//- for storage in this class.

template <class T, class sz>
class RealPoint : public Point<T, sz>
{
public:
  RealPoint (void);
  RealPoint (BinInput& bi);
  inline void load (int x, int y, int z, int r = 0);
  inline void load (double x, double y, double z, double r = 0.0);
  inline void load (double c[]);
  inline void load (float c[]);
  void get (double& x, double& y, double& z, double& r) const;
  inline void get (double& x, double& y, double& z) const;
  inline void get (float& x, float& y, float& z) const;
  inline void get (double c[]) const;
  inline void get (float c[]) const;
	inline void mul (double coef);
  void binDump(BinOutput& bo, double scale_ = 1.0);
  Bool binInit (BinInput& bi);

/*
  friend ostream&  operator<< (ostream& s, RealPoint<T, sz>& p)
    {  return s << (Point<T, sz> &) p; }
*/

#ifdef __GNUC__
  int operator== (const RealPoint<T, sz> & other) const;
  int operator!= (const RealPoint<T, sz> & other) const;
#else
	friend int operator== (const RealPoint<T, sz> & p1,
						   const RealPoint<T, sz> & p2);
	friend int operator!= (const RealPoint<T, sz> & p1,
						   const RealPoint<T, sz> & p2);
#endif

};

//-------------------------------------------------------------------------- 

class FloatPoint : public RealPoint <float, CoordNumber>
{
public:
  FloatPoint (void) : RealPoint <float, CoordNumber> () { }
  FloatPoint (BinInput& bi) : RealPoint <float, CoordNumber> (bi) { }
  int operator!= (const FloatPoint& p2) const;
};

class FloatPointW : public RealPoint <float, WeightedCoordNumber>
{
public:
  FloatPointW (void) : RealPoint <float, WeightedCoordNumber> () { }
  FloatPointW (BinInput& bi) : RealPoint <float, WeightedCoordNumber> (bi) { }
  int operator!= (const FloatPointW& p2) const;
};

class DoublePoint : public RealPoint <double, CoordNumber>
{
public:
  DoublePoint (void) : RealPoint <double, CoordNumber> () { }
  DoublePoint (BinInput& bi) : RealPoint <double, CoordNumber> (bi) { }
  int operator!= (const DoublePoint& p2) const;
};

class DoublePointW : public RealPoint <double, WeightedCoordNumber>
{
public:
  DoublePointW (void) : RealPoint <double, WeightedCoordNumber> () { }
  DoublePointW (BinInput& bi) : RealPoint <double, WeightedCoordNumber> (bi) { }
  int operator!= (const DoublePointW& p2) const;
};


//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

//. CLASS PointArray
//-
//- This template class implements a point array for a specified type of
//- point.  It allows indexing and I/O functionality.

template <class T>
class PointArray 
{
public:
  PointArray (int size = 16);
  PointArray (PointArray<T>& p);
  const PointArray<T> &operator= (const PointArray<T> &other);
  //  VIndex add (T &newVert);
  int add (T &newVert);
  int removeLast (void);
  int removeLasts (int n);
  int size (void) const;
  void reset (void);
  T &operator[] (int index);
  const T &operator[] (int index) const;
  void iter (void (* func) (T &elt));

  // @DT friend ostream& operator<< (ostream& s, PointArray<T>);
  void sort ( int (*compFun) (const void *, const void *));
  void removeDuplicates (void);
	void resize (int newSize);
protected:
  IterStack<T> iStack_;
};


//--------------------------------------------------------------------------

//. CLASS IntPArray
//-
//- This subclass of PointArray uses integer points to store the points.

class IntPArray : public PointArray<IntPoint>
{
public:
  IntPArray (int size = 16);

  friend ostream&  operator<< (ostream& s, const IntPArray& p);
};

//--------------------------------------------------------------------------
// class Interface of template RealPointArray

class RealPArray : public ProgressCallbackObject
{
public:
  RealPArray (void) { }
  virtual ~RealPArray (void);
  virtual int addPoint(double x, double y, 
                       double z, double r = 0) = 0;
  virtual int addPoint(double c[]) = 0;
  virtual int addPoint(float c[]) = 0;
  virtual int addPoint(RgiVector& vec) = 0;
  virtual int removeLastPoint (void) = 0;
  virtual int removeLastPoints (int n) = 0;
  virtual void loadPoint(int v, 
      double x, double y, double z, double r = 0) = 0;
  virtual void loadPoint(int v, double c[]) = 0;
  virtual void loadPoint(int v, float c[]) = 0;
  virtual void loadPoint(int v, RgiVector& bv) = 0;
      
  virtual void getPoint (int v, 
	  double& x, double& y, double& z, double& r) const = 0;
  virtual void getPoint (int v, 
          double& x, double& y, double& z) const = 0;
  virtual void getPoint (int v, double c[]) const = 0;
  virtual void getPoint (int v, float c[]) const = 0;
  virtual void getPoint (int v, RgiVector& vec) const = 0;
  virtual int size (void) const = 0;
  virtual void reset (void) = 0;
  virtual RealPArray * getNewRealPArray (void) = 0;
  virtual void sort ( int (*compFun) (const void *, const void *)) = 0;
  virtual void removeDuplicates (void) = 0;
  virtual void resize (int newSize) = 0;
  virtual Bool binInit ( BinInput& bi) = 0;
  virtual Bool binInit ( BinInput& bi, int (*test) (double *, void *), void * obj) = 0;
  virtual void binDump ( BinOutput& bo, int lastElem = -1) = 0;
  virtual void binDump ( BinOutput& bo,BitVector*,BitVector*) = 0;
  virtual RealPArray*  getNewEmptyRealPArray (int size = 16) = 0;
  static  RealPArray*  getNewEmptyRealPArray (BinInput& bi);
  virtual double getScale (void) = 0;
  virtual void setScale (double scale) = 0;
  virtual void truncate (int p) = 0;
  virtual int getClassId (void) = 0;
  double getAbsMax (void);
 
    //add by xinwei, 2005-05-04
  double getMaxRadii (void);

	void getBBox (double *min, double *max) const;
	void getInfo (VertexArray &);

  friend ostream&  operator<< (ostream& s, const RealPArray& p);
};

//. CLASS RealPointArray
//-
//- This subclass of PointArray uses real points to store the points.

template <class T>
class RealPointArray : public PointArray<T>, public RealPArray
{
public:
  RealPointArray (int size = 16);
  RealPointArray (RealPointArray<T>& p);
  RealPointArray (RealPArray& p);
  ~RealPointArray (void);
  int addPoint(double x, double y, double z, double r = 0); 
  int addPoint(double c[]);
  int addPoint(float c[]);
  int addPoint(RgiVector& bv);
  int addPoint(T& rp);
  int removeLastPoint (void);
  int removeLastPoints (int n);
  void loadPoint(int v, 
      double x, double y, double z, double r = 0);
  void loadPoint(int v, double c[]);
  void loadPoint(int v, float c[]);
  void loadPoint(int v, RgiVector& bv);
  void getPoint (int v, double& x, double& y, double& z, double& r) const;
  void getPoint (int v, double& x, double& y, double& z) const;
  void getPoint (int v, double c[]) const;
  void getPoint (int v, float c[]) const;
  void getPoint (int v, RgiVector& vec) const;
  virtual Bool binInit(BinInput& bi);
  virtual Bool binInit ( BinInput& bi, int (*test) (double *, void *), void * obj);
  virtual void binDump(BinOutput& bo, int lastElem = -1);
  virtual void binDump (BinOutput& bo,BitVector*,BitVector*) ;
  int size (void) const;
  virtual RealPArray * getNewRealPArray (void);
  virtual RealPArray * getNewEmptyRealPArray (int size = 16);
  void sort(int (*compFun) (const void *, const void *));
  void removeDuplicates (void);
	void resize (int newSize);
	void reset (void);
	double getScale (void);
	void setScale (double scale);
	void truncate (int p);
  int getClassId (void);
  T * getData (void) { return iStack_.getData(); }
  
protected:
  Magic magic_;
	double scale_;
  
};

//--------------------------------------------------------------------------


class FloatPArray : public RealPointArray<FloatPoint>
{
public:
  FloatPArray (int size = 16);
  FloatPArray (RealPArray& p);
  ~FloatPArray (void);
  RealPArray * getNewRealPArray (void);
  RealPArray * getNewEmptyRealPArray (int size = 16);
  static Bool isValid (int id);
};

class FloatPArrayW : public RealPointArray<FloatPointW>
{
public:
  FloatPArrayW (int size = 16);
  FloatPArrayW (RealPArray& p);
  ~FloatPArrayW (void);
  RealPArray * getNewRealPArray (void);
  RealPArray * getNewEmptyRealPArray (int size = 16);
  static Bool isValid (int id);
};

class DoublePArray : public RealPointArray<DoublePoint>
{
public:
  DoublePArray (int size = 16);
  DoublePArray (RealPArray& p);
  ~DoublePArray (void);
  RealPArray * getNewRealPArray (void);
  RealPArray * getNewEmptyRealPArray (int size = 16);
  static Bool isValid (int id);
};

class DoublePArrayW : public RealPointArray<DoublePointW>
{
public:
  DoublePArrayW (int size = 16);
  DoublePArrayW (RealPArray& p);
  ~DoublePArrayW (void);
  RealPArray * getNewRealPArray (void);
  RealPArray * getNewEmptyRealPArray (int size = 16);
  static Bool isValid (int id);
};

//--------------------------------------------------------------------------

class EdgeIndices // two vertex indices
{
public:
	int v1;
	int v2;
};

class TriangleIndices // three vertex indices
{
public:
	int v1;
	int v2;
	int v3;
};

class EdgeCoord 
{
public:
	DoublePoint p1;
	DoublePoint p2;
  void binDump (BinOutput& bo) { p1.binDump(bo), p2.binDump(bo); }
  Bool binInit (BinInput& bi)  { return p1.binInit(bi) && p2.binInit(bi); }
};

class TriangleCoord 
{
public:
	DoublePoint p1;
	DoublePoint p2;
	DoublePoint p3;	
  void binDump (BinOutput& bo) { p1.binDump(bo), p2.binDump(bo), p3.binDump(bo); }
  Bool binInit (BinInput& bi)  { return p1.binInit(bi) && p2.binInit(bi) && p3.binInit(bi); }
};

class EdgeCoordStack : public IterStackDumpable <EdgeCoord>
{
public:
  EdgeCoordStack (int size = 20) : IterStackDumpable<EdgeCoord>(size) { }
};

class VertexCoordStack : public IterStackDumpable <DoublePoint>
{
public:
  VertexCoordStack (int size = 20) : IterStackDumpable<DoublePoint>(size) { }
};

//--------------------------------------------------------------------------

#include <basic/points.hpp>

#endif /* __POINTS_H__ */
