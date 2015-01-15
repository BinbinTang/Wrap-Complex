#include <basic/comment.h>

#include "testint.h"

// #define PRINTBOXES

// #define ADDTRIANGLES

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------

IntersectionTester::
IntersectionTester (Complex *comp, TriangleType triType)
  : TristModifier (comp->getTrist()),
    n_ (comp->getTrist ()->numTri ()),
    comp_ (comp),
    triType_(triType),
    boxes_ (n_ + 4),
    trIndices_ (n_ + 4),
    intersects_ (comp->getTrist ()->maxTri ()),
    intervals_ (n_ + 4),
    points_ (n_ + 4),
    aux_ (n_ + 4)
{
  triangles_ = NULL;
  initVariables_ ();
  pEval_ = new SOS_FloatEvaluator(comp->getConverter());
}

//-------------------------------------------------------------------------

IntersectionTester::
IntersectionTester (Complex *comp, TriangleType triType,
		    BitVector * triangles)
  : TristModifier (comp->getTrist()),
    n_ (triangles->size ()),
    comp_ (comp),
    triType_(triType),
    boxes_ (n_ + 4),
    trIndices_ (n_ + 4),
    intersects_ (comp->getTrist ()->maxTri ()),
    intervals_ (n_ + 4),
    points_ (n_ + 4),
    aux_ (n_ + 4)
{
  triangles_ = triangles;
  initVariables_ (); 
  pEval_ = new SOS_FloatEvaluator(comp->getConverter());
}

//-------------------------------------------------------------------------

IntersectionTester::
IntersectionTester (int n)
  : n_ (n),
    comp_ (NULL),
    triType_(COMPLETE_TRI),
    boxes_ (n_ + 4),
    trIndices_ (n_ + 4),
    intersects_ (n_ + 4),
    intervals_ (n_ + 4),
    points_ (n_ + 4),
    aux_ (n_ + 4)
{
  initVariables_ ();
  initBoxes2_ ();
  pEval_ = NULL;
}

//-------------------------------------------------------------------------

IntersectionTester::
~IntersectionTester (void)
{
    delete pEval_;
}  

//-------------------------------------------------------------------------

void IntersectionTester::
init (void)
{
  int progB, progE, prog1;

  comp_->getConverter().truncPArray(*comp_->getPArray());

  getProgressBounds (progB, progE);
  prog1 = (progE - progB) / 5;
  
  if (triangles_ != NULL)
    initBoxes_ (triangles_);
  else
    initBoxes_ ();

  progressUpdate (0.2f);
  setProgressBounds (prog1, progE);
}

//-------------------------------------------------------------------------

void IntersectionTester::
initVariables_ (void)
{
  computeTwoThirds_ ();
  cutoff_ = n2third_;
  d_ = 2;
  debugFlag_ = FALSE;

  vzero(minCoord_);
  vzero(maxCoord_);
}  

//-------------------------------------------------------------------------

void IntersectionTester::
initBoxes_ (void)
{
  int i;
  
  curBox_ = 0;
  
	BitVectorIter iter(comp_->getFaceBV (triType_));
	for (iter.first(); !iter.isDone(); iter.next())
	  addBoxI(iter.curItem());

  n_ = curBox_;

  vcopy(minCoord_, boxes_[0].lo);
  vcopy(minCoord_, boxes_[0].hi);
  vcopy(maxCoord_, boxes_[n_ + 1].lo);
  vcopy(maxCoord_, boxes_[n_ + 1].hi);

  trIndices_[0] = 0;
  trIndices_[n_ + 1] = n_ + 1;
  
#ifdef PRINTBOXES
  printBox (0);
  printBox (n_ + 1);
  cout.flush ();
#endif

  for (i = 0; i <= n_ + 1; i++)
    intervals_[i] = i;
  quickSort_ (intervals_, 0, n_ + 1);

  for (i = 1; i <= n_ + 1; i++)
    points_[i] = intervals_[i];
}

//-------------------------------------------------------------------------

void IntersectionTester::
initBoxes_ (BitVector * triangles)
{
  int i;
  BitVectorIter triIter (*triangles);
  
  curBox_ = 0;

  for (triIter.first (); !triIter.isDone (); triIter.next ())
    addBoxI (triIter.curItem());
  
  n_ = curBox_;

  vcopy(minCoord_, boxes_[0].lo);
  vcopy(minCoord_, boxes_[0].hi);
  vcopy(maxCoord_, boxes_[n_ + 1].lo);
  vcopy(maxCoord_, boxes_[n_ + 1].hi);

  trIndices_[0] = 0;
  trIndices_[n_ + 1] = n_ + 1;
  
#ifdef PRINTBOXES
  printBox (0);
  printBox (n_ + 1);
  cout.flush ();
#endif

  for (i = 0; i <= n_ + 1; i++)
    intervals_[i] = i;
  quickSort_ (intervals_, 0, n_ + 1);

  for (i = 1; i <= n_ + 1; i++)
    points_[i] = intervals_[i];
}

/*************************************************************************/


/*************************************************************************/

void IntersectionTester::
addBoxI (FIndex fInd)
{
  VIndex a, b, c;
  double ax[3], bx[3], cx[3];
  double maxCoord;
  double minCoord;

  curBox_ ++;
  trIndices_[curBox_] = fInd;
  LI_Converter& converter = comp_->getConverter();
  RealPArray & rArray = *comp_->getPArray();
  // Load vertices...
  Triangle_ (OrTri(fInd, 0), a, b, c);
  rArray.getPoint(a, ax); converter.truncMul(ax);
  rArray.getPoint(b, bx); converter.truncMul(bx);
  rArray.getPoint(c, cx); converter.truncMul(cx);
  
  for (int k = 0; k <= 2; k++)
    {
      // find max for this box
      minCoord = boxes_[curBox_].lo[k] = findMin_ (ax[k], bx[k], cx[k]);
      maxCoord = boxes_[curBox_].hi[k] = findMax_ (ax[k], bx[k], cx[k]);
      
      if (minCoord < minCoord_[k])
	      minCoord_[k] = minCoord;
      if (maxCoord > maxCoord_[k])
	      maxCoord_[k] = maxCoord;
    }
 
}



/* initBoxes2 ************************************************************/
/* generates random boxes and stores them in B[L..R].  It also stores    */
/* dummy boxes at -infinity in B[L-1] and at +infinity in B[R+1].  The   */
/* box sides are chosen of length at most n^{2/3}, which causes roughly  */
/* a linear number of intersecting pairs, I think.                       */
/*************************************************************************/

void IntersectionTester::
initBoxes2_ (void)
{
  int i, k;
  int val1, val2;
  
  for (i = 1; i <= n_; i++) {
    for (k = 0; k <= 2; k++) {
      val1 = ((getRandom () >> 2) * (getRandom () >> 2)) % n_;
      val2 = 1 + getRandom () % n2third_;
      boxes_[i].lo[k] = val1;
      boxes_[i].hi[k] = val1 + val2;
    }
    trIndices_[i] = i;
  }

  for (k = 0; k <= 2; k++) {
    boxes_[0].lo[k] = -1;
    boxes_[0].hi[k] = -1;
    boxes_[n_ + 1].lo[k] = n_ + 10;
    boxes_[n_ + 1].hi[k] = n_ + 10;
  }
  trIndices_[0] = 0;
  trIndices_[n_ + 1] = n_ + 1;

  for (i = 1; i <= n_ + 1; i++)
    intervals_[i] = i;
  quickSort_ (intervals_, 0, n_ + 1);

  for (i = 1; i <= n_ + 1; i++)
    points_[i] = intervals_[i];
}

//-------------------------------------------------------------------------

int IntersectionTester::
performIntersect (void)
{
  boxPairs_ = triPairs_ = 0;
  
  intersects_.reset ();

  if (debugFlag_) {
    COMMENT("Calling Segment tree intersection algorithm...");
    printBoxes_ (intervals_, 0, n_ + 1);
  }
  findIntersections_ (intervals_, 1, n_, points_, 1, n_, 0, n_ + 1, 0);
  if (debugFlag_)
  {
    COMMENT("\n\n S2-algorithm:  ");
    COMMENT("Number of intersecting box pairs: " << boxPairs_);
    COMMENT("Number of intersecting triangle pairs: " << triPairs_);
    COMMENT("Number of intersecting triangles: " << intersects_.getNumSet ());
  }
  
  return triPairs_;
}

//-------------------------------------------------------------------------

int IntersectionTester::
performIntersect2 (void)
{
  boxPairs_ = triPairs_ = 0;
  intersects_.reset ();

  if (debugFlag_) {
    COMMENT("Calling All Pairs intersection algorithm...\n");
    printBoxes_ (intervals_, 0, n_ + 1);
  }
  allPairs_ (1, n_);
  if (debugFlag_)
  {
    COMMENT("\n\n All Pairs Algorithm:  \n");
    COMMENT("Number of intersecting box pairs: " << boxPairs_);
    COMMENT("Number of intersecting triangle pairs: " << triPairs_);
    COMMENT("Number of intersecting triangles: " << intersects_.getNumSet ());
  }
  
  return triPairs_;
}

//-------------------------------------------------------------------------

int IntersectionTester::
numIntersect (void)
{
  return intersects_.getNumSet ();
}

//-------------------------------------------------------------------------

int IntersectionTester::
doesIntersect (Index ix)
{
  return intersects_.test (ix);
}


/* findIntersections *****************************************************/
/* computes the intersecting box pairs by simulating the traversal of a  */
/* multi-layer segment tree.  The set of intervals is                    */
/* intervals[leftInt..rightInt], the set of points is                    */
/* points[leftPoint..rightPoint], the segment is                         */
/* [B[leftSeg]._,B[rightSeg]._), and the dimension is k (in [0, 2]).     */
/* The recursion ends when the number of intervals plus points does      */
/* no longer exceed the cutoff.  We use ._ as a short form for .Lo[k].   */
/*************************************************************************/
/* Invariants.  intervals[leftInt..rightInt] is sorted by d-th           */
/* coordinates and contains only intervals that have non-empty           */
/* intersection with the segment.  points[leftPoint..rightPoint] is      */
/* sorted by d-th coordinates and contains only points inside the        */
/* segment.  Both orders are destroyed for and within recursive calls    */
/* and reestablished at the end.                                         */
/*************************************************************************/

void IntersectionTester::
findIntersections_ (FixedArray<BoxIx> &intervals, int leftInt, int rightInt,
		   FixedArray<BoxIx> &points,    int leftPoint, int rightPoint,
		   BoxIx leftSeg, BoxIx rightSeg, Dim k)
{
    int index1, index2, index3, midPoint;
    BoxIx midSeg;
    int progB, progE, progDiff;
    int newLeftBound, newRightBound;
    int stepsCompleted = 0;
    
    progB = progE = 0;
    getProgressBounds (progB, progE);
	progDiff = progE - progB;

    if (debugFlag_)
      printNode_ (intervals, leftInt, rightInt,
		 points, leftPoint, rightPoint,
		 leftSeg, rightSeg, k);
    Assert (0 <= k && k <= d_);
    Assert (1 <= leftInt && rightInt <= n_  );
    Assert (1 <= leftPoint && rightPoint <= n_  );
    Assert (0 <= leftSeg && leftSeg <= n_ + 1);
    Assert (0 <= rightSeg && rightSeg <= n_ + 1);
    Assert (2 <= cutoff_);

    /* End recursion if a set or the segment is emtpy.  */
    if (isLoLessLo_ (rightSeg, leftSeg, k))
      {
	progressUpdate (1.0);
	return;
      }
    if ((leftInt > rightInt) || (leftPoint > rightPoint))
      return;
    if (k == d_)
      {
        scanContain_ (intervals, leftInt, rightInt,
		     points, leftPoint, rightPoint, k);
	progressUpdate (1.0);
	return;
      }
    if (rightPoint - leftPoint + 1 <= 2 ||
	(rightInt - leftInt + 1) + (rightPoint - leftPoint + 1) <= cutoff_)
      {
	scanIntersect_ (intervals, leftInt, rightInt,
		       points, leftPoint, rightPoint, k);
	progressUpdate (1.0);
	return;
      }

    /* Split sets and segment to prepare recursion.  Box midSeg is used */
    /* as a splitting element.                                          */

    /* Step 1:  split the points into two sets: ones intersecting the left
       segment, and one intersecting the right segment. */
    midPoint  = splitPoints_ (points, leftPoint, rightPoint, k, midSeg);

    /* Step 2:  split the intervals into three sets: ones intersecting
       the left segment, ones intersecting only the right segment, and
       ones containing the segment.  */
    splitIntervalsLeft_  (intervals, leftInt, rightInt,
			 leftSeg, midSeg, rightSeg, k, index2, index3);
    Assert (midPoint - leftPoint < rightPoint - leftPoint + 1);
    Assert (rightPoint - midPoint + 1 < rightPoint - leftPoint + 1);

    /* Do computations now for progress updates */
    int numPoints  = rightPoint - leftPoint + 1;
    int numLeftP   = midPoint   - leftPoint;
    int numRightP  = rightPoint - midPoint  + 1;
    int numInts    = rightInt   - leftInt   + 1;
    int numLeftI   = index2     - leftInt;
    int numRightI  = index3     - ((2 * numLeftI) / 3);   // A guess...
    int numCoverI  = rightInt   - index3 + 1;
    int numNCoverI = index3     - leftInt;

    /* Compute total number of steps. */

#define RECURSECOST(S,T)   ((((S)==0) || ((T)==0)) ? 0 : (3 * (S+T)))
#define STEP1COST     numPoints
#define STEP2COST     numInts
#define STEP3COST     RECURSECOST(numLeftI,numLeftP)
#define STEP4COST     numNCoverI
#define STEP5COST     RECURSECOST(numRightI,numRightP)
#define STEP6COST     numPoints
#define STEP7COST     RECURSECOST(numPoints,numCoverI)
#define STEP8COST     RECURSECOST(numPoints,numCoverI)
#define STEP9COST     numInts
    
    int totalSteps;
    totalSteps = STEP1COST + STEP2COST + STEP3COST + STEP4COST + STEP5COST
      + STEP6COST + STEP7COST + STEP8COST + STEP9COST;

    stepsCompleted += (STEP1COST + STEP2COST);
    progressUpdate ( (float) ((stepsCompleted * 1.0) / totalSteps) );
    
    /* Step 3.  Recurse Left */
    newLeftBound = progB
      + ((stepsCompleted * progDiff) / totalSteps);
    newRightBound = newLeftBound + ((STEP3COST * progDiff) / totalSteps);
    setProgressBounds (newLeftBound, newRightBound);

    findIntersections_ (intervals, leftInt, index2-1,
		       points, leftPoint, midPoint-1,
		       leftSeg, midSeg, k);

    setProgressBounds (progB, progE);
    stepsCompleted += STEP3COST;
    progressUpdate ( (float) ((stepsCompleted * 1.0) / totalSteps) );

    /* Step 4.  Split intervals, preparing for right recursion. */
    splitIntervalsRight_ (intervals, leftInt, index2, index3, midSeg, k,
			 index1);
    stepsCompleted += STEP4COST;
    progressUpdate ( (float) ((stepsCompleted * 1.0) / totalSteps) );

    /* Step 5.  Recurse Right */
    newLeftBound = progB
      + ((stepsCompleted * progDiff) / totalSteps);
    newRightBound = newLeftBound + ((STEP5COST * progDiff) / totalSteps);
    setProgressBounds (newLeftBound, newRightBound);

    findIntersections_ (intervals, index1,  index3-1,
		       points, midPoint,  rightPoint,
		       midSeg, rightSeg, k);

    setProgressBounds (progB, progE);
    stepsCompleted += STEP5COST;
    progressUpdate ( (float) ((stepsCompleted * 1.0) / totalSteps) );

    /* Step 6.  Merge Points after recursion is finished. */
    mergePoints_ (points, leftPoint, midPoint, rightPoint);
    stepsCompleted += STEP6COST;
    progressUpdate ( (float) ((stepsCompleted * 1.0) / totalSteps) );

    /* Step 7.  Recurse for Ispan, points.  */
    newLeftBound = progB
      + ((stepsCompleted * progDiff) / totalSteps);
    newRightBound = newLeftBound + ((STEP7COST * progDiff) / totalSteps);
    setProgressBounds (newLeftBound, newRightBound);

    findIntersections_ (intervals, index3,  rightInt,
		       points, leftPoint, rightPoint,
		       0, n_ + 1, k + 1);

    setProgressBounds (progB, progE);
    stepsCompleted += STEP7COST;
    progressUpdate ( (float) ((stepsCompleted * 1.0) / totalSteps) );

    /* Step 7.  Recurse for Ispan, points with roles reversed.  */
    newLeftBound = progB
      + ((stepsCompleted * progDiff) / totalSteps);
    newRightBound = newLeftBound + ((STEP8COST * progDiff) / totalSteps);
    setProgressBounds (newLeftBound, newRightBound);

    findIntersections_ (points, leftPoint, rightPoint,
		       intervals, index3,  rightInt,
		       0, n_ + 1, k + 1);

    setProgressBounds (progB, progE);
    stepsCompleted += STEP8COST;
    progressUpdate ( (float) ((stepsCompleted * 1.0) / totalSteps) );

    /* Step 9.  Merge intervals back together. */
    mergeIntervals_ (intervals, leftInt, index1, index3, rightInt);
    progressUpdate (1.0);
}


/* splitIntervalsLeft ****************************************************/
/* The intervals in intervals[L..R] are assumed to have non-empty        */
/* intersection with the segment [B[leftSeg]._, B[rightSeg]._).  This    */
/* segment consists of two pieces: [B[leftSeg]._, B[midSeg]._) union     */
/* [B[midSegs]._, B[rightSeg]._).  Depending on the intersection with    */
/* the pieces we split intervals[L..R] into three parts:                 */
/*                                                                       */
/*   intervals[L..b-1]:  intervals that intersect left segment,          */
/*   intervals[b..c-1]:  intervals that intersect right but not left     */
/*                       segment,                                        */
/*   intervals[c..R]  :  intervals that contain the segment.             */
/*                                                                       */
/* The split is stable maintaining order in d-th coordinates.  Indices   */
/* b, c are output parameters.                                           */
/*************************************************************************/

void IntersectionTester::
splitIntervalsLeft_ (FixedArray<BoxIx> &intervals, int L, int R,
		    BoxIx leftSeg, BoxIx midSeg, BoxIx rightSeg,
		    int k, int &b, int &c)
{
    int i, x, y;

    b = L;  x = L;  y = R;
    for (i = L; i <= R; i++) {
      if (!isLoLessLo_ (intervals[i], midSeg, k))          // Type 1
	{
	  aux_[x]  = intervals[i];
	  x++;
	}
      else if (isLoLessLo_ (leftSeg, intervals[i], k) ||
	       isHiLessLo_ (intervals[i], rightSeg, k))    // Type 2
	{
	  intervals[b] = intervals[i];
	  (b)++;
	}
      else                                                // Type 3
	{
	  aux_[y]  = intervals[i];
	  y--;
	}
    }
    Assert (((b)-L)+(x-L)+(R-y) == R-L+1);
    Assert (x-1 < y+1); 

    for (i = L; i <= x-1; i++)
      intervals[(b)+i-L] = aux_[i];
    c = (b)+x-L;
    for (i = R; i >= y+1; i--)
      intervals[(c)+R-i] = aux_[i];
}


/* splitIntervalsRight ***************************************************/
/* modifies the preparation of intervals from left to right recursion.   */
/* In particular, intervals[L..b-1] is split into                        */
/* intervals[L..a-1] and intervals[a..b-1], and the later is             */
/* merged with intervals[b..c-1].  The result is                         */
/*                                                                       */
/*   intervals[L..a-1]:  intervals that intersect left but not           */
/*                             right segment,                            */
/*   intervals[a ..c-1]:  intervals that intersect right segment.        */
/*                                                                       */
/* The split is stable maintaining order by d-th coordinates.  Index a   */
/* is output parameter.                                                  */
/*************************************************************************/

void IntersectionTester::
splitIntervalsRight_ (FixedArray<BoxIx> &intervals, int L, int b, int c,
		     BoxIx midSeg, int k, int &a)
{
    int i, j, x, y;

    a = L;  x = L;
    for (i = L; i <= b-1; i++) {
      if (isHiLessLo_ (intervals[i], midSeg, k))       // Type 1
	{
	  intervals[a] = intervals[i];
	  (a)++;
	}
      else                                        // Type 2 
	{
	  aux_[ x] = intervals[i];
	  x++;
	}
    }
    Assert (((a)-L)+(x-L) == b-L);

    // Merge a..b and b..c.
    
    j = b;  y = L;
    for (i = a; i <= c-1; i++) {
      if (j >= c)
	{
	  intervals[i] = aux_[y];
	  y++;
	}
      else if (y >= x)
	{
	  intervals[i] = intervals[j];
	  j++;
	}
      else if (isLoLessLo_ (intervals[j], aux_[y], d_))
	{
	  intervals[i] = intervals[j];
	  j++;
	}
      else
	{
	  intervals[i] = aux_[y];
	  y++;
	}
    }
    Assert (y == x && j == c);
}


/* mergeIntervals ********************************************************/
/* merges intervals[L..a-1], intervals[a..c-1],                          */
/*        and intervals[c..R]                                            */
/* back into a single sorted array.  The three arrays are assumed to be  */
/* sorted by d-th coordinates of low points.                             */
/*************************************************************************/

void IntersectionTester::
mergeIntervals_ (FixedArray<BoxIx> &intervals, int L, int a, int c, int R)
{
    int i, j, x, y;

    i = L;  j = a;
    for (x = L; x <= c-1; x++) {
      if (i >= a)
	{
	  aux_[x] = intervals[j];
	  j++;
	}
      else if (j >= c)
	{
	  aux_[x] = intervals[i];
	  i++;
	}
      else if (isLoLessLo_ (intervals[i], intervals[j], d_))
	{
	  aux_[x] = intervals[i];
	  i++;
	}
      else
	{
	  aux_[x] = intervals[j];
	  j++;
	}
    }
    Assert (i == a && j == c);

    j = c;  y = L;
    for (i = L; i <= R; i++) {
      if      (j >= R+1)
	{
	  intervals[i] = aux_[y];
	  y++;
	}
      else if (y >= x  )
	{
	  intervals[i] = intervals[j];
	  j++;
	}
      else if (isLoLessLo_ (intervals[j], aux_[y], d_))
	{
	  intervals[i] = intervals[j];
	  j++;
	}
      else
	{
	  intervals[i] = aux_[y];
	  y++;
	}
    }
    Assert (j == R+1 && y == x);
}


/* splitPoints ***********************************************************/
/* splits points[L..R] into Pleft = points[L..p-1]                       */
/*                     and  Pright = points[p..R].                       */
/* To guarantee progress and achieve good balance on the average, the    */
/* pivot x is chosen randomly and as the median of three.  The split is  */
/* stable maintaining order in d-th coordinates.  x is output parameter. */
/*************************************************************************/

int IntersectionTester::
splitPoints_ (FixedArray<BoxIx> &points, int L, int R, int k, BoxIx &x)
{
    int r, j, p, a;

    Assert (R-L+1 >= 3);
    r = L + getRandom () % (R-L+1);
    if (r == R)
      r--;
    if (r == L)
      r++;
    x = medianOf3_ (points[r-1], points[r], points[r+1], k);

    p = L;  a = L;
    for (j = L; j <= R; j++) {
      if (isLoLessLo_ (points[j], x, k))
	{
	  points[p] = points[j];
	  p++;
	}
      else
	{
	  aux_[a] = points[j];
	  a++;
	}
    }
    Assert ((p-L)+(a-L) == R-L+1);
    for (j = L; j <= a-1; j++)
      points[p+j-L] = aux_[j];

    Assert (L < p && p < R);
    return p;
}


/* mergePoints ***********************************************************/
/* merges points[L..m-1] and points[m..R] into points[L..R].  The two    */
/* arrays are assumed to be sorted by d-th coordinates of lo points.     */
/*************************************************************************/

void IntersectionTester::
mergePoints_ (FixedArray<BoxIx> &points, int L, int m, int R)
{
    int i, j, p;

    for (i = L; i <= m-1; i++)
      aux_[i] = points[i];
    i = L;  j = m;
    for (p = L; p <= R; p++) {
      if (i > m-1)
	{
	  points[p] = points[j];
	  j++;
	}
      else if (j > R  )
	{
	  points[p] = aux_[i];
	  i++;
	}  
      else if (isLoLessLo_ (aux_[i], points[j], d_))
	{
	  points[p] = aux_[i];
	  i++;
	}
      else
	{
	  points[p] = points[j];
	  j++;
	}
    }
    Assert (i == m && j == R+1);
}


/* scanContain ***********************************************************/
/* checks all pairs in                                                   */
/*      intervals[leftInt..rightInt] x points[leftPoint..rightPoint]     */
/* and reports (bI, bP) if                                               */
/*   (1) bI != bP,                                                       */
/*   (2) d-th interval of bI contains d-th lo coordinate of bP, and      */
/*   (3) projections to dimensions beyond d have non-empty intersection. */
/* intervals, points are assumed to be sorted by d-th coordinates of     */
/* low points.  The number of intersecting pairs is returned.  Watch     */
/* unconventional construction of the case statement with optional       */
/* breaks.                                                               */
/*************************************************************************/

void IntersectionTester::
scanContain_ (FixedArray<BoxIx> &intervals, int leftInt, int rightInt,
	     FixedArray<BoxIx> &points, int leftPoint, int rightPoint, int k)
{
  int i, p, q;

  Assert (k == d_);

  p = leftPoint;
  for (i = leftInt; i <= rightInt; i++) {
    while ((p <= rightPoint) && (!isLoLessLo_ (intervals[i], points[p], d_)))
      p++;
    if (p <= rightPoint) {
      Assert (intervals[i] != points[p]);
      q = p;
      while ((q <= rightPoint) && (isLoLessHi_ (points[q], intervals[i], d_))) {
	switch (d_) {
	case 0:
	  if (!doesIntersectInterval_ (intervals[i], points[q], 1))
	    break;
	case 1:
	  if (!doesIntersectInterval_ (intervals[i], points[q], 2))
	    break;
	case 2:
	  boxPairs_++;
		if (debugFlag_)
		  {
		    COMMENT("Type 1:\n");
		    printPair_ (intervals[i], points[q]);
		  }
	    
	  if (testTriangleIntersect_ (trIndices_[intervals[i]],
				     trIndices_[points[q]]))
	    {
	      if (debugFlag_)
		{
		  printTriPair_ (trIndices_[intervals[i]],
				trIndices_[points[q]]);
		  printPair_ (intervals[i], points[q]);
		}
	      intersects_.set (trIndices_[intervals[i]]);
	      intersects_.set (trIndices_[points[q]]);
	      triPairs_ ++;
	    }
	  break;
	}
	q++;
      }
    }
  }
}


/* scanIntersect *********************************************************/
/* is same as ScanContain except it checks for interval intersection in  */
/* the d-th coordinate direction.  The pair (bI, bP) is reported if      */
/*   (1) bI != bP,                                                       */
/*   (2) d-th interval of bI intersects d-th interval of bP,             */
/*   (3) k-th interval of bI contains k-th lo coordinate of bP, and      */
/*   (4) projections to dimensions between k and d have non-emtpy        */
/*       intersection.                                                   */
/* intervals, points are assumed to be sorted by d-th coordinates of     */
/* low points.  The number of intersecting pairs is returned.  Watch     */
/* unconventional construction of the case statement with optional       */
/* breaks.                                                               */
/*************************************************************************/

void IntersectionTester::
scanIntersect_ (FixedArray<BoxIx> &intervals, int leftInt, int rightInt,
	       FixedArray<BoxIx> &points, int leftPoint, int rightPoint, int k)
{
  BoxIx b;
  int   i, j, p, q, dim;

  Assert ( (0 <= k) && (k < d_) && (d_ <= 2));

  i = leftInt;
  p = leftPoint;
  dim = 3 - d_;
  while (i <= rightInt && p <= rightPoint) {
    if ((intervals[i] == points[p]) ||
	isLoLessLo_ (intervals[i], points[p], d_)) {
      b = intervals[i];
      i++;
      q = p;
      if (points[q] == b)
	q++;
      while ((q <= rightPoint) && isLoLessHi_ (points[q], b, d_)) {
	switch (k) {
	case 0:
	  if (!doesIntersectInterval_ (b, points[q], dim))
	    break;
	case 1:
	  if ( doesContainPoint_ (b, points[q], k)) {
	    boxPairs_++;
		if (debugFlag_)
		  {
		    COMMENT("Type 2:\n");
		    printPair_ (b, points[q]);
		  }
	    
	    if (testTriangleIntersect_ (trIndices_[b],
				       trIndices_[points[q]]))
	      {
		if (debugFlag_)
		  {
		    printTriPair_ (trIndices_[b], trIndices_[points[q]]);
		    printPair_ (b, points[q]);
		  }
		intersects_.set (trIndices_[b]);
		intersects_.set (trIndices_[points[q]]);
		triPairs_ ++;
	      }
	    break;
	  }
	}
	q++;
      }
    }
    else
      {
	Assert (isLoLessLo_ (points[p], intervals[i], d_));
	b = points[p];
	p++;
	j = i;
	Assert (intervals[j] != b);
	while ((j <= rightInt) && isLoLessHi_ (intervals[j], b, d_)) {
	  switch (k) {
	  case 0:
	    if (!doesIntersectInterval_ (intervals[j], b, dim))
	      break;
	  case 1:
	    if ( doesContainPoint_ (intervals[j], b, k)) {
	      boxPairs_++;
		if (debugFlag_)
		  {
		    COMMENT("Type 3:\n");
		    printPair_ (intervals[j], b);
		  }
		
	      if (testTriangleIntersect_ (trIndices_[intervals[j]],
					 trIndices_[b]))
		{
		  if (debugFlag_)
		    {
		      printTriPair_ (trIndices_[intervals[j]], trIndices_[b]);
		      printPair_ (intervals[j], b);
		    }
		  intersects_.set (trIndices_[intervals[j]]);
		  intersects_.set (trIndices_[b]);
		  triPairs_ ++;
		}
	      break;
	    }
	  }
	  j++;
	}
      }
  }
}


/*************************************************************************/

void IntersectionTester::
printTriPair_ (FIndex ix1, FIndex ix2)
{
  /*
  VIndex x, y, z;

  COMMENT("Intersecting pair:  \n");
  tr_->printTriangle (cout, ix1);
  Triangle_ (OrTri (ix1, 0), x, y, z);
  COMMENT("\t(" << (*pArray_)[x]);
  COMMENT("\t(" << (*pArray_)[y]);
  COMMENT("\t(" << (*pArray_)[z]);
  tr_->printTriangle (cout, ix2);
  Triangle_ (OrTri (ix2, 0), x, y, z);
  COMMENT("\t(" << (*pArray_)[x]);
  COMMENT("\t(" << (*pArray_)[y]);
  COMMENT("\t(" << (*pArray_)[z]);
  */
}
		  
/*************************************************************************/

int IntersectionTester::
testTriangleIntersect_ (FIndex ix1, FIndex ix2)
{
  OrTri ef1 (ix1, 0);
  OrTri ef2 (ix2, 0);
  int numVertsShared;

  if (not pEval_)
    return TRUE;     // So that testing with performIntersect2 works...
  
  numVertsShared = computeVertsShared_ (ef1, ef2);
  //  tr_->printTriangle (cout, ef1);
  //  tr_->printTriangle (cout, ef2);
  //  COMMENT("Verts Shared = " << numVertsShared << endl);
  
  switch (numVertsShared) {
  case 0:
    return testTriangleIntersect0_ (ef1, ef2);
  case 1:
    return testTriangleIntersect1_ (ef1, ef2);
  default:
    return FALSE;
  }
}

/*************************************************************************/

int IntersectionTester::
computeVertsShared_ (OrTri &ef1, OrTri &ef2)
{
  VIndex u1, u2, u3;
  VIndex v1, v2, v3;
  
  Triangle_ (ef1, u1, u2, u3);
  Triangle_ (ef2, v1, v2, v3);

  if      (u1 == v1) /* do nothing */ ;
  else if (u1 == v2) ef2 = Enext_  (ef2);
  else if (u1 == v3) ef2 = Enext2_ (ef2);
  else {
    ef1 = Enext_(ef1);
    if      (u2 == v1) /* do nothing */;
    else if (u2 == v2) ef2 = Enext_ (ef2);
    else if (u2 == v3) ef2 = Enext2_ (ef2);
    else {
      ef1 = Enext_ (ef1);
      if      (u3 == v1) /* do nothing */;
      else if (u3 == v2) ef2 = Enext_ (ef2);
      else if (u3 == v3) ef2 = Enext2_ (ef2);
      else {
	ef1 = Enext_ (ef1);
	return 0;
      }
    }
  }

  // One vertex the same.
      
  Triangle_ (ef1, u1, u2, u3);
  Triangle_ (ef2, v1, v2, v3);

  Assert (u1 == v1);
  if (u2 == v2)
    return 2;
  else if (u2 == v3)
    {
      ef2 = Enext_(Sym_ (ef2));
      return 2;
    }
  else if (u3 == v2)
    {
      ef1 = Enext_(Sym_(ef1));
      return 2;
    }
  else if (u3 == v3)
    {
      ef1 = Enext_(Sym_(ef1));
      ef2 = Enext_(Sym_(ef2));
      return 2;
    }
  else
    return 1;
}

/*************************************************************************/

int IntersectionTester::
testTriangleIntersect0_ (OrTri ef1, OrTri ef2)
{
  VIndex a, b, c;
  VIndex x, y, z;
  VIndex tmp;

  RealPArray& rArray = *comp_->getPArray();
  
  Triangle_ (ef1, a, b, c);
  Triangle_ (ef2, x, y, z);

  int test1 = pEval_->positive3 (rArray, a, b, c, x);
  int test2 = pEval_->positive3 (rArray, a, b, c, y);
  int test3 = pEval_->positive3 (rArray, a, b, c, z);

  if ((test1 == test2) && (test2 == test3))
    return FALSE;

  int test4 = pEval_->positive3 (rArray, x, y, z, a);
  int test5 = pEval_->positive3 (rArray, x, y, z, b);
  int test6 = pEval_->positive3 (rArray, x, y, z, c);

  if ((test4 == test5) && (test5 == test6))
    return FALSE;

  // Orient vertices correctly...

#define SWAP(T1,T2) { tmp = T1; T1 = T2; T2 = tmp; }
  
  if (test1 == test2)
  {
    SWAP(x,z);
  }
  else if (test1 == test3)
  {
    SWAP(x,y);
  }
  
  if (test4 == test5)
  {
    SWAP(a,c);
  }
  else if (test4 == test6)
  {
    SWAP(a,b);
  }

#undef SWAP

  int test7  = pEval_->positive3 (rArray, a, b, x, y);
  int test8  = pEval_->positive3 (rArray, a, b, x, z);
  int test9  = pEval_->positive3 (rArray, a, c, x, y);
  int test10 = pEval_->positive3 (rArray, a, c, x, z);

  if (test7 != test8)
    return TRUE;
  if (test9 != test10)
    return TRUE;
  if (test7 != test9)
    return TRUE;
  Assert (test8 == test10);
  return FALSE;
}

/*************************************************************************/

int IntersectionTester::
testTriangleIntersect1_ (OrTri ef1, OrTri ef2)
{
  int a, b, c;
  int x, y, z;
  RealPArray& rArray = *comp_->getPArray();

  Triangle_ (ef1, a, b, c);
  Triangle_ (ef2, x, y, z);
  Assert (a == x);

  int test1 = pEval_->positive3 (rArray, a, b, c, y);
  int test2 = pEval_->positive3 (rArray, a, b, c, z);
  
  if (test1 == test2)
    return FALSE;

  int test3 = pEval_->positive3 (rArray, a, y, z, b);
  int test4 = pEval_->positive3 (rArray, a, y, z, c);
  
  if (test3 == test4)
    return FALSE;

  int test5 = pEval_->positive3 (rArray, b, c, y, z);
  int test6 = pEval_->positive3 (rArray, b, c, z, a);
  int test7 = pEval_->positive3 (rArray, b, c, a, y);
  
  if ((test5 == test6) && (test6 == test7))
    return TRUE;

  int test8  = pEval_->positive3 (rArray, y, z, a, b);
  int test9  = pEval_->positive3 (rArray, y, z, b, c);
  int test10 = pEval_->positive3 (rArray, y, z, c, a);
  
  if ((test8 == test9) && (test9 == test10))
    return TRUE;
  else
    return FALSE;
}

/* quickSort *************************************************************/
/* sorts the index array A[L..R] in non-decreasing order of the box lo   */
/* points B[A[]].Lo[d].                                                  */
/*************************************************************************/

void IntersectionTester::
quickSort_ (FixedArray<BoxIx> &A, int L, int R)
{
    int q;

    if (L < R) { q = split_ (A, L, R);
		 quickSort_ (A, L, q);
		 quickSort_ (A, q+1, R);
    }
}


/* Split *****************************************************************/
/* chooses a random pivot element and splits A[L..R] at its value.       */
/*************************************************************************/

int IntersectionTester::
split_ (FixedArray<BoxIx> &A, int L, int R)
{
    int   i, j, p;
    BoxIx x, aux;

    p = L + getRandom () % (R-L+1);
    Assert (L <= p && p <= R);
    x = A[p];  A[p] = A[L];  A[L] = x;

    i = L-1;
    j = R+1;
    while (1) {
      do
	j--;
      while (isLoLessLo_ (x, A[j], d_));
      do
	i++;
      while (isLoLessLo_ (A[i], x, d_));

      if (i < j) {
	aux = A[i];  A[i] = A[j];  A[j] = aux;
      }
      else
	return j;
    }
}


/* medianOf3 *************************************************************/
/* returns the median of three box lo points.  The three input boxes are */
/* assumed to be pairwise different.                                     */
/*************************************************************************/

int IntersectionTester::
medianOf3_ (BoxIx a, BoxIx b, BoxIx c, int k)
{
   BoxIx aux;

   Assert (a != b && b != c && c != a);
   if (isLoLessLo_ (c, a, k)) { aux = a;  a = c;  c = aux; }
   if (isLoLessLo_ (b, a, k))   return a;
   if (isLoLessLo_ (b, c, k))   return b;
                                return c;
}

/*************************************************************************/

void IntersectionTester::
computeTwoThirds_ (void)
{
  int n2third = 1, n1third = 1;

  while (n2third*n1third < n_) {
    n2third++;
    if (n1third*n1third < n2third)  n1third++;
  }
  Assert (1 <= n2third);
  n2third_ = n2third;
}

/* printBoxes ************************************************************/
/* prints all boxes with indices in A[L..R].                             */
/*************************************************************************/

void IntersectionTester::
printBoxes_ (FixedArray<BoxIx> &A, int L, int R)
{
/*
    int i;

    Assert ((&A == &points_) || (&A == &intervals_));
    COMMENT("\n\n  Boxes in " << ((&A == &intervals_) ? "I" : "P")
	 << "[" << L << "," << R << "]:\n");
    for (i = L; i <= R; i++) {
      COMMENT(" ");
      printBox (A[i]);
    }
*/
}


/* printBox **************************************************************/
/* prints a single box.                                                  */
/*************************************************************************/

void IntersectionTester::
printBox_ (BoxIx x)
{
/*
    COMMENT("    Box " << x << ": [" << *boxes_[x].Lo[0] << " "
	 << *boxes_[x].Hi[0] << ") x [" <<  *boxes_[x].Lo[1] << " "
	 << *boxes_[x].Hi[1] << ") x [" <<  *boxes_[x].Lo[2] << " "
	 << *boxes_[x].Hi[2] << ").\n");
*/
}


/* printPair *************************************************************/
/* prints a pair of boxes: (*boxes_)[i] and (*boxes_)[j].                */
/*************************************************************************/

void IntersectionTester::
printPair_ (BoxIx i, BoxIx j)
{
  COMMENT("\n  Pair:");
  printBox_ (i);
  COMMENT("  Pair:");
  printBox_ (j);
  cout.flush ();
  Assert (doesIntersectInterval_ (i, j, 0)
	  && doesIntersectInterval_ (i, j, 1)
	  && doesIntersectInterval_ (i, j, 2));
}


/* printNode *************************************************************/
/* prints the parameters with which STree is called.                     */
/*************************************************************************/

void IntersectionTester::
printNode_ (FixedArray<BoxIx> &intervals, int leftInt, int rightInt,
	   FixedArray<BoxIx> &points, int leftPoint, int rightPoint,
	   BoxIx leftSeg, BoxIx rightSeg, int k)
{
  COMMENT("\n\nSEGMENT TREE:  dimension k = " << k << ",  #layers = "
       << d_ << ".\n");
  COMMENT("  segment = [" << boxes_[leftSeg].lo[k] << ", "
       << boxes_[rightSeg].lo[k] << "), from boxes " <<  leftSeg
       << ", " << rightSeg << ";\n");
  COMMENT("  intervals[" << leftInt << ".." << rightInt << "], points["
       << leftPoint << ".." << rightPoint << "];\n");
  printBoxes_ (intervals, leftInt, rightInt);
  printBoxes_ (points, leftPoint, rightPoint);
}


/* allPairs **************************************************************/
/* checks boxes in B[L..R] and counts and reports intersecting pairs.    */
/*************************************************************************/

void IntersectionTester::
allPairs_ (BoxIx L, BoxIx R)
{
    BoxIx i, j;

    for (i = L; i <= R-1; i++)
        for (j = i+1; j <= R; j++)
	  {
            if (doesIntersectInterval_ (i, j, 0)
		&& doesIntersectInterval_ (i, j, 1)
		&& doesIntersectInterval_ (i, j, 2))
	      {
		boxPairs_++;
		if (debugFlag_)
		  printPair_ (i, j);
	      }
	    
	    if (testTriangleIntersect_ (trIndices_[i],
				       trIndices_[j]))
	      {
		if (debugFlag_)
		  printPair_ (i, j);
		intersects_.set (trIndices_[i]);
		intersects_.set (trIndices_[j]);
		triPairs_ ++;
	      }
	    }
}


/* isSorted **************************************************************/
/* checks whether or not the sequence of boxes referenced in A[L..R] is  */
/* sorted by d-th coordinates of lo points.                              */
/*************************************************************************/

int IntersectionTester::
isSorted_ (FixedArray<BoxIx> &A, int L, int R)
{
    int i;

    for (i = L; i <= R-1; i++)
	if (!isLoLessLo_ (A[i], A[i+1], d_))  return 0;
    return 1;
}

