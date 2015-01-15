/*************************************************************************/
/* This header file ties together various files for computing pairs      */
/* of intersecting boxes in 3-dimensional space.                         */
/*************************************************************************/
/* box.c,                                                                */
/*************************************************************************/

#ifndef __RGI_TESTINT_H__
#define __RGI_TESTINT_H__

#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <limits.h>


#include <basic/farray.h>
#include <basic/tritype.h>
#include <sos/sosfloat.h>
#include <geometry/comp.h>

/*************************************************************************/

class Box 
  {
  public:
   double lo[3], hi[3];
  };

typedef int BoxIx;


/* box.c *****************************************************************/
/* Class IntersectionTester - performs intersection test                 */
/*************************************************************************/

class IntersectionTester : public TristModifier
{

public:

  IntersectionTester (Complex *comp, TriangleType triType);
  IntersectionTester (Complex *comp, TriangleType triType, BitVector * triangles);
  IntersectionTester (int n);
  
  ~IntersectionTester (void);

  void init (void);
  
  int performIntersect (void);     // Segment tree algorithm
  int performIntersect2 (void);    // All Pairs algorithm

  //- doesInterect
  int doesIntersect (Index ix);
     //. returns 1 if triangle ix intersects some other triangle, and
     //.         0 otherwise
  
  void debugOn (void)  { debugFlag_ = TRUE; }
  void debugOff (void) { debugFlag_ = FALSE; }

  //- numIntersect
  int numIntersect (void);
     //. returns the number of intersecting triangles

  // Needs to be public for callback...
  void addBoxI (FIndex fInd);

  BitVector * getIntersectBV (void) { return &intersects_; }

  

private:
  int n_;                              /* Number of boxes.                  */
  int d_;                              /* Number of segment tree layers.    */
  int cutoff_;                         /* Threshold for strategy change.    */
  int n2third_;
  int debugFlag_;
  int boxPairs_, triPairs_;
  BitVector * triangles_;
  
  Complex *comp_;                     /* The Complex */
  TriangleType triType_;
  
  FixedArray<Box> boxes_;             /* Array of boxes,     (0..n+1).    
					 B[0]   stores dummy box -infty.
					 B[n+1] stores dummy box +infty.   */
  FixedArray<FIndex> trIndices_;      /* The triangle index corresponding
					 to a particular box. (0..n+1)     */
  BitVector intersects_;              /* A bitvector indicating which
					 triangles intersect.              */

  FixedArray<BoxIx> intervals_;       /* Array of intervals, (0..n+1).     */
  FixedArray<BoxIx> points_;          /* Array of points,    (0..n+1).     */
  FixedArray<BoxIx> aux_;             /* Auxiliary array to stably split   */
                                      /*    arrays.                        */
  SOS_FloatEvaluator * pEval_;
  
  // Variables used in filling the boxes.
  int curBox_;
  double minCoord_[3], maxCoord_[3];
  
  void initVariables_ (void);

  /*************************************************************************/
  /* routines for finding intersecting pairs of boxes by recursive         */
  /* traversal of a segment tree.                                          */
  /*************************************************************************/

  void findIntersections_ (FixedArray<BoxIx> &intervals,
			  int leftInt, int rightInt,
			  FixedArray<BoxIx> &points,
			  int leftPoint, int rightPoint,
			  BoxIx leftSeg, BoxIx rightSeg, Dim k);

  void splitIntervalsLeft_ (FixedArray<BoxIx> &intervals, int L, int R,
			   BoxIx leftSeg, BoxIx midSeg, BoxIx rightSeg,
			   int k, int &b, int &c);

  void splitIntervalsRight_ (FixedArray<BoxIx> &intervals, int L, int b, int c,
			    BoxIx midSeg, int k, int &a);

  void mergeIntervals_ (FixedArray<BoxIx> &intervals,
		       int L, int a, int c, int R);

  int splitPoints_ (FixedArray<BoxIx> &points,
		   int leftPoint, int R, int k, BoxIx &x);

  void mergePoints_ (FixedArray<BoxIx> &points, int L, int m, int R);

  void scanContain_ (FixedArray<BoxIx> &intervals, int leftInt, int rightInt,
		    FixedArray<BoxIx> &points, int leftPoint, int rightPoint,
		    int k);

  void scanIntersect_ (FixedArray<BoxIx> &intervals, int leftInt, int rightInt,
		      FixedArray<BoxIx> &points, int leftPoint, int rightPoint,
		      int k);

  void printTriPair_ (FIndex ix1, FIndex ix2);
  int testTriangleIntersect_  (FIndex ix1, FIndex ix2);
  int testTriangleIntersect0_ (OrTri  ef1, OrTri  ef2);
  int testTriangleIntersect1_ (OrTri  ef1, OrTri  ef2);

  int computeVertsShared_    (OrTri &ef1, OrTri &ef2);

  // Sorting and Searching routines

  void quickSort_ (FixedArray<BoxIx> &A, int L, int R);
  int split_ (FixedArray<BoxIx> &A, int L, int R);
  int medianOf3_ (BoxIx a, BoxIx b, BoxIx c, int k);

  /* io.c ******************************************************************/
  /* contains printing functions.                                          */
  /*************************************************************************/

  //. initBoxes
  void initBoxes_ (void);
  void initBoxes_ (BitVector * triangles);
  void initBoxes2_ (void);
       //- initBoxes fills the boxes using the real triangles, while
       //- initBoxes2 generates random boxes.

  double findMin_ (double val1, double val2, double val3);
  double findMax_ (double val1, double val2, double val3);
  
  void computeTwoThirds_ (void);
  
  void printBoxes_ (FixedArray<BoxIx> &A, int L, int R);
  void printBox_ (BoxIx x);
  void printPair_ (BoxIx i, BoxIx j);
  void printNode_ (FixedArray<BoxIx> &intervals, int leftInt, int rightInt,
		FixedArray<BoxIx> &points, int leftPoint, int rightPoint,
                BoxIx leftSeg, BoxIx rightSeg, int k);


  /* check.c ***************************************************************/
  /* contains checking functions.                                          */
  /*************************************************************************/

  void allPairs_ (BoxIx L, BoxIx R);
  int isSorted_ (FixedArray<BoxIx> &A, int L, int R);

  Bool isLoLessLo_(BoxIx i, BoxIx j, Dim k);
  Bool isHiLessLo_(BoxIx i, BoxIx j, Dim k);
  Bool isLoLessHi_(BoxIx i, BoxIx j, Dim k);
  Bool doesContainPoint_(BoxIx i, BoxIx j, Dim k);
  Bool doesIntersectInterval_(BoxIx i, BoxIx j, Dim k);

  /*************************************************************************/
  /*                           TO BE CHANGED.                              */
  /*************************************************************************/
  /* Extensions:                                                           */
  /* (1) Generate the boxes from point triplets, similar to the actual     */
  /*     application of the software.                                      */
  /*************************************************************************/
};

inline Bool IntersectionTester::
isLoLessLo_(BoxIx i, BoxIx j, Dim k)
{
  return ((boxes_[i].lo[k] < boxes_[j].lo[k]) ||
         ((boxes_[i].lo[k] == boxes_[j].lo[k]) && (i < j)));
}

inline Bool IntersectionTester::
isHiLessLo_(BoxIx i, BoxIx j, Dim k)
{
  return ((boxes_[i].hi[k] < boxes_[j].lo[k]) ||
         ((boxes_[i].hi[k] == boxes_[j].lo[k]) && (i < j)));
}


inline Bool IntersectionTester::
isLoLessHi_(BoxIx i, BoxIx j, Dim k)
{
  return ((boxes_[i].lo[k] < boxes_[j].hi[k]) ||
         ((boxes_[i].lo[k] == boxes_[j].hi[k]) && (i < j)));
}

inline Bool IntersectionTester::
doesContainPoint_(BoxIx i, BoxIx j, Dim k)
{
  return (!isLoLessLo_ (j, i, k) && isLoLessHi_ (j, i, k));
}

inline Bool IntersectionTester::
doesIntersectInterval_ (BoxIx i, BoxIx j, Dim k)
{
  return (!isHiLessLo_ (i, j, k) && !isHiLessLo_ (j, i ,k));
}

inline double IntersectionTester::
findMin_ (double val1, double val2, double val3)
{
  if (val2 < val1) val1 = val2;
  return MIN(val1, val3);
}

inline double IntersectionTester::
findMax_ (double val1, double val2, double val3)
{
  if (val2 > val1) val1 = val2;
  return MAX(val1, val3);
}


#endif // __TESTINT_H__
