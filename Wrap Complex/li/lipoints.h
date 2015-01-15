/*
 * points.h -- Class definitions for all point and point array classes.
 *
 * author -- Mike Facello
 *
 * date -- 10/7/96
 */

#ifndef __RGI_LIPOINTS_H__
#define __RGI_LIPOINTS_H__

#include <basic/points.h>
#include <li/lia.h>
#include <li/li.h>

/*  Number of coordinates per vertex, plus constants for accessing the
    correct elements of the arrays */


//--------------------------------------------------------------------------

//. CLASS LI_Point
//-
//- A subclass of Point using long integers for coordinates.

class LI_Point : public Point<LI_Coord, WeightedCoordNumber>
{
public:
  LI_Point (void);
  void load(const LI_Coord& x, const LI_Coord& y, 
	    const LI_Coord& z, const LI_Coord& r = 0);
  void alloc (void);
  void free (void);
  void load(double x[4], int w, int a, int computeSumSq = 0); 
  void computeSumSquares (void);
  int binInit (BinInput& bi);
  void binDump(BinOutput& bo);

  friend ostream&  operator<< (ostream& s, LI_Point& p);
};


//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

//. CLASS LI_PArray
//-
//- This subclass of PointArray uses long integer points to store the points.

class LI_PArray : public PointArray<LI_Point>, public ProgressCallbackObject
{
public:
  LI_PArray (int size = 10, int length = 16, int decimals = 0);
  LI_PArray (LI_PArray& pArray);
  LI_PArray (RealPArray& rArray, int length, int decimals);
  ~LI_PArray (void);

  // create real point version of the array
  RealPArray *getNewRealArray (void);
  // create real point version of the array from input instead of self-initializing
  RealPArray *getNewRealArray (BinInput &bi);
  // self-initialize from input
  Bool binInit (BinInput& bi);
  // dump self
  void binDump (BinOutput& bo);
  // dump real point array as it was an LI_PArray
  void binDump (RealPArray * rArray, BinOutput& bo);

  void setConverter (LI_Converter *);  
  LI_Converter *getConverter (void);

  // NOTE:  The following methods return the index of the newly added
  //        point.  But, it can be assumed that if the points are added
  //        in order, the indices will be 1, 2, 3, ...  (until vertex
  //        deletion is needed.)
  
  int addPoint (int x, int y, int z, int r = 0);
  int addPoint (float x, float y, float z, float r = 0.0);
  int addPoint (double x, double y, double z, double r = 0.0);
  int addPoint (char *x, char *y, char *z, char *r = "0");
  void loadPoint (int v, double x, double y, double z, double r = 0.0);
  void getPoint (int v, double& x, double& y, double& z, double& r);
  
  friend ostream&  operator<< (ostream& s, LI_PArray& p);

private:
  Magic magic_;
  LI_Converter *liConvert_;
  LI_Point tmpPoint_;
};

//--------------------------------------------------------------------------

#endif /* __LIPOINTS_H__ */
