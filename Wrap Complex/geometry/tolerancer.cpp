//
// Copyright 1996-98. Raindrop Geomagic, Inc.
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

#define USE_COLOR_SPACE_MAPPING 1

#include <iostream>
#include <basic/points.h>
//Alan Comment
//#include <base/rgicolorspace.h>
#include <geometry/tolerancer.h>
#include <geometry/boxes.h>
#include <geometry/segmenttree.h>
#include <basic/comment.h>
#include <basic/miscmath.h>
#include <assert.h>

/* Constants */
const int kRescaleFactorPowerOf2 = 20;
const int kRescaleFactor = 1 << kRescaleFactorPowerOf2; // size of integer lattice
const double kStretchFactor = 1.1;     // 10% bigger cubes around elements for first phase

/* Macros */
static double _distArg;

#define Square(x) ((x)*(x))
#define Dot(v1,v2) (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])
#define Dist(n) ((_distArg = (n)) > 0.0 ? _distArg : -_distArg)
#define AssignBinOpVector(a,b,op,c)  \
     a[0] = b[0] op c[0];            \
     a[1] = b[1] op c[1];            \
     a[2] = b[2] op c[2];

#define BinOpAndAssignVector(a,op,b) \
     a[0] op b[0];                   \
     a[1] op b[1];                   \
     a[2] op b[2];

/* Class Methods */

Tolerancer::
Tolerancer (int maxElement,BitVector *tbv)
: computedDistances_(FALSE),
  maxElement_(maxElement),
  evaluator_(NULL),
  points_(NULL),
  distances_(NULL),
  closestElements_(NULL),
  closestPts_ (NULL),
  closestPtBaryCoords_ (NULL),
  closestPtEdges_ (NULL),
  doComputeClosestPts_ (FALSE),
  doComputeClosestPtBaryCoords_ (FALSE),
//  tbv_(tbv),
  iwmap_(NULL),
  wimap_(NULL)
{

	Assert(tbv);
	tbv_ = tbv;
}

//----------------------------------------------------------------------------------

Tolerancer::
~Tolerancer(void) 
{
  int i;

	if(distances_ != NULL) 
    delete [] distances_;
	if(closestElements_ != NULL) 
    delete [] closestElements_;
  if (closestPts_ != NULL)
  {
    for (i = 0; i < numPoints_; i++)
      delete [] closestPts_[i];
    delete [] closestPts_;
  }
  if (closestPtBaryCoords_ != NULL)
  {
    for (i = 0; i < numPoints_; i++)
      delete [] closestPtBaryCoords_[i];
    delete [] closestPtBaryCoords_;
  }
  if (closestPtEdges_ != NULL)
    delete closestPtEdges_;

  if(wimap_) delete [] wimap_;
  if(iwmap_) delete [] iwmap_;
  delete tbv_;
}

//----------------------------------------------------------------------------------

void Tolerancer::
setPointData (RealPArray *points,BitVector *pointbv)
{
	int i;
    points_= points;
	pbv_ = pointbv;
//	numPoints_ = points_->size();
	numPoints_ = pbv_->getNumSet();
	BitVectorIter bvi(*pbv_);
	if(iwmap_)
		delete[] iwmap_;
	iwmap_ = new int [numPoints_];
	for(bvi.first(),i=0;!bvi.isDone();bvi.next(),i++)
		iwmap_[i] = bvi.curItem();
	computedDistances_ = FALSE;
}

//----------------------------------------------------------------------------------

void Tolerancer::
setCallbackFuncs (RgiObject * evaluator, ComputeBBoxCB bboxCB, 
                  ComputeElemBBoxCB elemBBoxCB, ComputeDistCB distCB,
									ComputeSideCB signCB, ValidIndexCB isValidCB)
{
  evaluator_ = evaluator;
  bboxCB_ = bboxCB;
  elemBBoxCB_ = elemBBoxCB;
  distCB_ = distCB;
	signCB_ = signCB;
  isValidCB_ = isValidCB;
	computedDistances_ = FALSE;
}

//----------------------------------------------------------------------------------

int Tolerancer::
computeTolerance (void)
{
	double surfaceBBox[2][kNumDim];
	double pointsBBox[2][kNumDim];
	double dataBBox[2][kNumDim];
	double cubeLen, diagonal, factor;
	boxesT *pointsBoxes, *surfaceBoxes;
	int numIntersections, numWithoutDist;
	double maxError;
	int i;

  if (!evaluator_)
    return FALSE;
  if (!points_)
    return FALSE;
	if (numPoints_ == 0)
		return FALSE;

	if(computedDistances_) 
    return TRUE; // already did compute distances

  if (closestPtEdges_ == NULL)
    closestPtEdges_ = new EdgeCoordStack;
  else
    closestPtEdges_->reset ();

	COMMENT(numPoints_ << " points, " << maxElement_ << " surfaceboxes.");
 
	// Prep 1
	if(distances_ == NULL) {
		distances_ = new double [numPoints_];
		closestElements_ = new int [numPoints_];
    if (doComputeClosestPts_)
    {
      closestPts_ = new double* [numPoints_];
      for (i = 0; i < numPoints_; i++)
        closestPts_[i] = new double[kNumDim];
    }
    if (doComputeClosestPtBaryCoords_)
    {
      closestPtBaryCoords_ = new double* [numPoints_];
      for (i = 0; i < numPoints_; i++)
        closestPtBaryCoords_[i] = new double[kNumDim];
    }
	}
	for(i = 0; i < numPoints_; i++) 
    distances_[i] = -1.0;

	(evaluator_->*bboxCB_)(surfaceBBox);
	putBoundingBoxAroundPoints_(pointsBBox);
	cubeLen = putBoundingCubeAroundBoxes_(surfaceBBox, pointsBBox, dataBBox, diagonal); 
	
	COMMENT( "Bounding Cube side is " << cubeLen);

	// degenerate surface/point set
	if(cubeLen == 0.0) {
		cubeLen = 1.0;
		diagonal = 1.0;
	}

	factor = kRescaleFactor / cubeLen;

	pointsBoxes = CreateBoxesT(numPoints_);
	surfaceBoxes = CreateBoxesT(maxElement_);
	putEmptyBoxesAroundPoints_(pointsBoxes, factor, dataBBox[0]);
	putBoundingCubesAroundElements_(surfaceBoxes, factor, dataBBox[0]);
	
	COMMENT("Prep 1 done.");

  progressUpdate (0.1f);

	// Intersection 1
	numIntersections = CountUsingSegmentTreeAlg(pointsBoxes, surfaceBoxes, 
		NULL, this, &Tolerancer::computeDistanceSquaredCB, TRUE);
	COMMENT("Int 1:  "<< numIntersections << " intersections");
  progressUpdate (0.5f);

	// Prep 2
	numWithoutDist = countNegativeValues_(distances_, numPoints_);
	COMMENT(numWithoutDist << " without distances.");
	findDistanceFailedPoints_(pointsBoxes, surfaceBoxes, numWithoutDist);
	
	putDistanceBoxesAroundPoints_(pointsBoxes, factor, dataBBox[0]);
	putBoundingBoxesAroundElements_(surfaceBoxes, factor, dataBBox[0]);
	COMMENT("Prep 2 done.");
  progressUpdate (0.6f);

	// Intersection 2
	numIntersections = CountUsingSegmentTreeAlg(pointsBoxes, surfaceBoxes, 
		NULL, this, &Tolerancer::computeDistanceSquaredCB, FALSE);
	COMMENT("Int 2:  "<< numIntersections << " intersections");
	numWithoutDist = countNegativeValues_(distances_, numPoints_);
  progressUpdate (0.9f);

	COMMENT(numWithoutDist << " without distances.");

	// Should never happen (we guarantee this in Int 2)
	assert(numWithoutDist == 0);

	// taking square roots
	for(i = 0; i < numPoints_; i++) 
    distances_[i] = sqrt(distances_[i]);

	// Print out max error (take this out later)
	maxError = findAbsMax_(distances_, numPoints_);
	COMMENT ("Max error is "<< maxError << " (" 
    << maxError/diagonal*100.0 << "% of diagonal of bounding cube).");

	// compute signs for distances
	// NEED TO ADD TEST FOR CLOSED SURFACE (MANIFOLD)
	computeSigns_();

	// we computed distances!
	computedDistances_ = TRUE;

  // Update the closest point edge stack;
  if (doComputeClosestPts_)
    computeClosestPtEdges_();

	// deallocating
	FreeBoxes(pointsBoxes);
	FreeBoxes(surfaceBoxes);
  progressUpdate (1.0f);
	return TRUE;
}

//----------------------------------------------------------------------------------

Bool Tolerancer::
getDistances(double distances[])
{
	int i;

	if(computedDistances_) {
		for(i = 0; i < numPoints_; i++) {
			distances[i] = Abs(distances_[i]);
		}
		return TRUE;
	} else {
		return FALSE;
	}
}

//----------------------------------------------------------------------------------

Bool Tolerancer::
getStatistics (double &maxDist, double &avgDist, double &stdDev, int printOutput)
{
  int i, j;
  //double absDist;
  if (! computedDistances_)
    return FALSE;
	const int numLevels = 8;
	const double InchToMillimeterFactor = 25.4;
	double mmLevels[numLevels] =
		{ 0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 1.0, 3.0 };
	double inchLevels[numLevels];
	int numByInchLevel[numLevels];
	int numByMMLevel[numLevels];

	maxDist = 0.0;
  avgDist = 0.0;
  stdDev = 0.0;
	for (j = 0; j < numLevels; j++)
	{
		numByMMLevel[j] = 0;
		numByInchLevel[j] = 0;
		inchLevels[j] = mmLevels[j] / InchToMillimeterFactor;
	}

  for (i = 0; i < numPoints_; i++)
  {
    double absDist = Abs (distances_[i]);
    if (absDist > maxDist)
      maxDist = absDist;
    avgDist += absDist;
		for (j = 0; j < numLevels; j++)
			if ((j == numLevels-1) || ((absDist >= mmLevels[j]) && (absDist < mmLevels[j+1])))
			{
				numByMMLevel[j] ++;
				break;
			}
		for (j = 0; j < numLevels; j++)
			if ((j == numLevels-1) || ((absDist >= inchLevels[j]) && (absDist < inchLevels[j+1])))
			{
				numByInchLevel[j] ++;
				break;
			}
  }
  avgDist /= numPoints_;
  
  // Compute Standard Deviation
  for (i = 0; i < numPoints_; i++)
  {
    double absDist = Abs (distances_[i]);
    stdDev += (absDist - avgDist) * (absDist - avgDist);
  }
  stdDev /= (numPoints_ - 1);
  stdDev = sqrt (stdDev);

	if (printOutput)
	{
		cout << "Statistics for Tolerancing:" << endl<< endl;
		cout << "Max = " << maxDist<< endl;
		cout << "Avg = " << avgDist<< endl;
		cout << "StdDev = " << stdDev<< endl;
		cout << "Breakdown by levels (in mm):"<< endl;
		for (j = 0; j < numLevels; j++)
			if (j == numLevels-1)
				cout << "\tAbove " << mmLevels[j] << ": " << numByMMLevel[j] << "."<< endl;
			else
				cout << "\t[" << mmLevels[j] << ", " << mmLevels[j+1] << ") : " << numByMMLevel[j] << "."<< endl;
		cout << "Breakdown by levels (in inches):"<< endl;
		for (j = 0; j < numLevels; j++)
			if (j == numLevels-1)
				cout << "\tAbove " << inchLevels[j] << ": " << numByInchLevel[j] << "."<< endl;
			else
				cout << "\t[" << inchLevels[j] << ", " << inchLevels[j+1] << ") : " << numByInchLevel[j] << "."<< endl;
	}

  return TRUE;
}

//----------------------------------------------------------------------------------

Bool Tolerancer::
getSignedDistances(double distances[])
{
	int i;	

	if(computedDistances_) {
		for(i = 0; i < numPoints_; i++) {
			distances[i] = distances_[i];
		}
		return TRUE;
	} else {
		return FALSE;
	}
}

//----------------------------------------------------------------------------------

Bool Tolerancer::
getDistanceRGBs(double rgb[][3])
{
	int i;
	double absmax;

	if (computedDistances_) {

		absmax = findAbsMax_(distances_, numPoints_);

		// no error - make sure division doesn't mess up
		if(absmax == 0.0) {

			absmax = 1.0;
		}

		double accept = absmax/20.0;

		for (i = 0; i < numPoints_; i++) {

#if USE_COLOR_SPACE_MAPPING

			computeColorSpace(distances_[i], rgb[i], accept, -absmax, absmax);

#else
			// CURRENTLY ASSUMES SIGNED DISTANCE
			computeHotColormapRGB_(Abs(distances_[i] / absmax), rgb[i]);
			//computeBlueRedSignedRGB_(distances_[i] / absmax, rgb[i]);
#endif
		}

		return TRUE;
	} 
	else {

		return FALSE;
	}
}

//----------------------------------------------------------------------------------

Bool Tolerancer::
getDistanceRGBs(double rgb[][3], double minVal, 
				double maxVal, double accept)
{
	int i;
	//double absmax;
	double range;

	if (maxVal < minVal)
		maxVal = minVal;

	range = maxVal - minVal;

	if (range == 0.0)
		range = 1.0;

	if (computedDistances_) {

		for (i = 0; i < numPoints_; i++) {

#if USE_COLOR_SPACE_MAPPING

			computeColorSpace(distances_[i], rgb[i], accept, minVal, maxVal);

#else

			// CURRENTLY ASSUMES SIGNED DISTANCE

			curVal = Abs(distances_[i]);

			if (curVal < minVal)
				curVal = minVal;

			curVal = curVal - minVal;
      
			if (curVal > range)
				curVal = range;

			computeHotColormapRGB_(Abs(curVal / range), rgb[i]);
			//computeBlueRedSignedRGB_(distances_[i] / absmax, rgb[i]);
#endif
		}

		return TRUE;
	} 
	else {

		return FALSE;
	}
}
//----------------------------------------------------------------------------------

inline double Tolerancer::
findAbsMax_(double array[], int num)
{
	int i;
	double max;

	assert(num >= 1);
	max = Abs(array[0]);
	for(i = 1; i < num; i++) {
		if(Abs(array[i]) > max) max = Abs(array[i]);
	}
	return max;
}


//----------------------------------------------------------------------------------

void Tolerancer::
computeColorSpace(const double intensity, double rgb[3], const double accept,
				  const double minVal, const double maxVal)
{

// Alan Comment
//	RgiColorSpace cs;

	float r = 0.0;
	float g = 0.0;
	float b = 0.0;

	double val = intensity;

	//
	// Clamp to the min and max values.
	//

	BOOL maxClamp = FALSE;
	BOOL minClamp = FALSE;

	if (val > maxVal) {

		val = maxVal;
		maxClamp = TRUE;
	}
	else if (val < minVal) {

		val = minVal;
		minClamp = TRUE;
	}

	//
	// Calculate the RGB triad for the input value.
	//

	if (val > accept || maxClamp) {

		// If the value is greater than the acceptable range, then
		// we fade the color hue from green-yellow to red.  The hue
		// values for this are 0.25 (green-yellow) to 0.0 (red).
		// Use the same saturation and luminance values as the
		// nurb tolerancer.

		double percent = 0.0;
		const double range = maxVal - accept;

		if (range > 0.0) {

			percent = (val - accept)/range;
		}
		else {

			percent = 1.0;
		}

		const double hue = 0.25 - percent*0.25;

//		cs.setHSL(float(hue), 1.0f, 0.5f);

//		cs.getRGB(r, g, b);
	}
	else if (val < -accept || minClamp) {

		// If the value is less than the acceptable range, then
		// we fade the color hue from green-cyan to blue.  The hue
		// values for this are 0.41667 (green-cyan) to 0.6667 (blue).
		// Use the same saturation and luminance values as the
		// nurb tolerancer.

		double percent = 0.0;
		const double range = -accept - minVal;

		if (range > 0.0) {

			percent = 1.0 - (val - minVal)/range;
		}
		else {

			percent = 1.0;
		}

		const double hue = percent*0.25 + 0.4166666667;

//		cs.setHSL(float(hue), 1.0f, 0.5f);

//		cs.getRGB(r, g, b);
	}
	else {

		// If the distance is in the acceptable range, get the neutral
		// RGB color from the color space object.

//		cs.getNeutralRGB(r, g, b);
	}

	//
	// Assign the calculated RGB to the passed in RGB triad.
	//

	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;
}

//----------------------------------------------------------------------------------

inline void Tolerancer::
computeHotColormapRGB_(double intensity, double rgb[3])
{
	assert(intensity >= 0.0 || intensity <= 1.0); 
	/* .375 = 3/8 4 is 8/2 2.666666666 is 8/3 */
	rgb[1] = rgb[2] = 0.0;
  rgb[0] = intensity > .375 ? 1.0 : 2.666666666666666*intensity;
  intensity -= .375;
  if(intensity < 0.0) return;
  rgb[1] = intensity > .375 ? 1.0 : 2.666666666666666*intensity;
  intensity -= .375;
  if(intensity < 0.0) return;
  rgb[2] = 4.0 * intensity;
}

//----------------------------------------------------------------------------------
// blue for outside, red for inside, black for surface
inline void Tolerancer::
computeBlueRedSignedRGB_(double intensity, double rgb[3])
{
	assert(intensity >= -1.0 || intensity <= 1.0); 
	if(intensity >= 0.0) {
		rgb[0] = rgb[1] = 0.0;
		rgb[2] = intensity;
	} else {
		rgb[0] = -intensity;
		rgb[1] = rgb[2] = 0.0;
	}
}	

//----------------------------------------------------------------------------------

void Tolerancer::
putBoundingBoxAroundPoints_(double bbox[2][kNumDim])
{
	int i, j;
	double point[kNumDim];

	points_->getPoint(iwmap_[0], point);
	for(i = 0; i < kNumDim; i++) {
		bbox[0][i] = bbox[1][i] = point[i];
	}
	// find mins and maxes
	for(j = 1; j < numPoints_; j++) {
		for(i = 0; i < kNumDim; i++) {
			points_->getPoint(iwmap_[j], point);
			if(point[i] < bbox[0][i]) bbox[0][i] = point[i];
			if(point[i] > bbox[1][i]) bbox[1][i] = point[i];
		}
	}
}

//----------------------------------------------------------------------------------

// here, bounding cube is defined by the minimum point
// object is put on the corner of cube
// i.e. cube[0] and the side as computed.
// it is NOT stored as element cubes in "cube"
double Tolerancer::
putBoundingCubeAroundBoxes_(double box1[2][kNumDim], 
													  double box2[2][kNumDim], 
														double cube[2][kNumDim],
													  double &diagonal)
{
	int i;
	double len[kNumDim];
	double side;

	// find bounding box of boxes
	for(i = 0; i < kNumDim; i++) {
		cube[0][i] = (box1[0][i] < box2[0][i]) ? box1[0][i] : box2[0][i];
		cube[1][i] = (box1[1][i] > box2[1][i]) ? box1[1][i] : box2[1][i];
	}
	
	// find max length, i.e. cube side
	side = findLengths_(cube, len);
	diagonal = sqrt(Square(len[0])+Square(len[1])+Square(len[2]));
	return side;
}

//----------------------------------------------------------------------------------

inline double Tolerancer::
findLengths_(double box[2][kNumDim], double lengths[kNumDim])
{
	int i;
	double max;

	// find lengths
	for(i = 0; i < kNumDim; i++) {
		lengths[i] = box[1][i] - box[0][i];
	}

	// find max length
	max = lengths[0];
	if(lengths[1] > max) max = lengths[1];
	if(lengths[2] > max) max = lengths[2];
	return max;
}

//----------------------------------------------------------------------------------

void Tolerancer::
putEmptyBoxesAroundPoints_(boxesT *boxes, double factor,
													 double mins[kNumDim])
{
	int i, j;
	boxT *boxArray;
	double point[kNumDim];

	boxArray = GetBoxArray(boxes);
	for(i = 0; i < numPoints_; i++) {
		boxArray[i].num = i;
		points_->getPoint(iwmap_[i], point);  
		for(j = 0; j < kNumDim; j++) {
			boxArray[i].ints[j].lo = boxArray[i].ints[j].hi =
					(int)((point[j] - mins[j])*factor);	
		}
	}
}

//----------------------------------------------------------------------------------

void Tolerancer::
putBoundingCubesAroundElements_(boxesT *boxes,double factor, 
																double mins[kNumDim])
{
	boxT *boxArray;
	double elemBBox[2][kNumDim];
	double len[kNumDim];
	double halfLen, midpoint;
	int i,j;

	boxArray = GetBoxArray(boxes);



  // Need an "isValid" check here, and change to "maxElement_".
	BitVectorIter itr(*tbv_);
    for(itr.first(),i=0;!itr.isDone();itr.next(),i++) {
		boxArray[i].num = itr.curItem();
		if(!(evaluator_->*elemBBoxCB_)(itr.curItem(), elemBBox))
			continue; 
		halfLen = findLengths_(elemBBox, len);  
		halfLen *= kStretchFactor / 2.0; /* /2 is for halflen */
		for(j = 0; j < kNumDim; j++) {
			midpoint = (elemBBox[0][j] + elemBBox[1][j]) / 2.0;
			boxArray[i].ints[j].lo = 
				((int)((midpoint - halfLen - mins[j])*factor))-1;
			boxArray[i].ints[j].hi = 
				((int)((midpoint + halfLen - mins[j])*factor))+1;
		}
	}	
}

//----------------------------------------------------------------------------------

void Tolerancer::
putBoundingBoxesAroundElements_(boxesT *boxes,double factor, 
																double mins[kNumDim])
{
	boxT *boxArray;
	double elemBBox[2][kNumDim];
	int i,j;

  boxArray = GetBoxArray(boxes);

  // Need an "isValid" check here, and change to "maxElement_".
	BitVectorIter itr(*tbv_);
    for(itr.first(),i=0;!itr.isDone();itr.next(),i++) {
		boxArray[i].num = itr.curItem();
		(evaluator_->*elemBBoxCB_)(itr.curItem(), elemBBox);
		for(j = 0; j < kNumDim; j++) {
		  boxArray[i].ints[j].lo = ((int)((elemBBox[0][j] - mins[j])*factor))-1;
      boxArray[i].ints[j].hi = ((int)((elemBBox[1][j] - mins[j])*factor))+1;  
    }
  }
}

//----------------------------------------------------------------------------------

void Tolerancer::
putDistanceBoxesAroundPoints_(boxesT *boxes, double factor, double mins[])
{
  int i, j;
  boxT *boxArray;
  double dist;
  double point[kNumDim];

  boxArray = GetBoxArray(boxes);
 
	for(i = 0; i < numPoints_; i++) {
    boxArray[i].num = i;      /* need to do this again as boxes have moved */
		dist = distances_[i];
		assert(dist >= 0.0);
    dist = sqrt(dist);
		points_->getPoint(iwmap_[i], point);  
    for(j = 0; j < kNumDim; j++) {
      boxArray[i].ints[j].lo =
				((int)((point[j] - dist - mins[j])*factor))-1;
      boxArray[i].ints[j].hi = 
				((int)((point[j] + dist - mins[j])*factor))+1;
    }
  }
}

//----------------------------------------------------------------------------------

inline void Tolerancer::
computeDistanceSquaredCB(int pointNum, int elemNum)
{
	double dist;
	double point[kNumDim];
  double closestPt[kNumDim];
  double closestPtBaryCoords[kNumDim];
  EdgeCoord ec;

//	points_->getPoint(pointNum + 1, point);
	points_->getPoint(iwmap_[pointNum], point);
	dist = (evaluator_->*distCB_)(elemNum, point,
    doComputeClosestPts_ ? closestPt : NULL,
    doComputeClosestPtBaryCoords_ ? closestPtBaryCoords : NULL);
	if(dist < distances_[pointNum] ||
		distances_[pointNum] == -1.0) 
  {
		distances_[pointNum] = dist;
		closestElements_[pointNum] = elemNum;
    if (doComputeClosestPts_)
      for (int i = 0; i < kNumDim; i++)
        closestPts_[pointNum][i] = closestPt[i];
    if (doComputeClosestPtBaryCoords_)
      for (int i = 0; i < kNumDim; i++)
        closestPtBaryCoords_[pointNum][i] = closestPtBaryCoords[i];
	}
}

//----------------------------------------------------------------------------------

void Tolerancer::
findDistanceFailedPoints_(boxesT *pointsBoxes, boxesT *surfaceBoxes,
													int numWithoutDist)
{
	boxT temp;
	boxT *boxArray;
	int numRounds;
	int numIntersections;

	if(numWithoutDist == 0) return;

	for(numRounds = 0; 
			numWithoutDist > 0;
			numRounds++) {
		COMMENT("\tRound " << numRounds << ":  " << numWithoutDist << " points");
		scaleUpBoxes_(surfaceBoxes, 1);
    moveFailedVerticesToFront_(pointsBoxes);  
    boxArray = GetBoxArray(pointsBoxes);
    SetNumBoxes(pointsBoxes, numWithoutDist);
    temp = boxArray[numWithoutDist]; /* first box after failed ones */
    boxArray[numWithoutDist] = GetPosInfBox(pointsBoxes);
		numIntersections = CountUsingSegmentTreeAlg(pointsBoxes, surfaceBoxes, 
			NULL, this, &Tolerancer::computeDistanceSquaredCB, TRUE);
		COMMENT("\t" << numIntersections << " intersections");

    /* fix last box */
    boxArray[numWithoutDist] = temp;
    numWithoutDist = countNegativeValues_(distances_, numPoints_);
  }  
  SetNumBoxes(pointsBoxes, numPoints_);
  scaleDownBoxes_(surfaceBoxes, numRounds);
}

//----------------------------------------------------------------------------------

inline int Tolerancer::
countNegativeValues_(double array[], int num)
{
	int i;
	int numNeg = 0;


	for(i = 0; i < num; i++) {
		if(array[i] < 0.0) numNeg++;
	}
	return numNeg;
}

//----------------------------------------------------------------------------------

// basically the largest length that makes
// sense within our grid is one power higher
// than our cubelen.  so we keep that as
// our max in order to avoid overflow problems
// this should not happen except for extremely
// pathological cases, but we'll keep code robust
void Tolerancer::
scaleUpBoxes_(boxesT *boxes, int numPowersOf2)
{
  int i, j;
  int numBoxes;
  int halfLen, mid;
  boxT *boxArray;
	int len;
	int maxLenBeforeScale;

  maxLenBeforeScale = 1 << (kRescaleFactorPowerOf2 - numPowersOf2 + 1);
	boxArray = GetBoxArray(boxes);
  numBoxes = GetNumBoxes(boxes);
  
  numPowersOf2--;  /* for half length */
  for(i = 0; i < numBoxes; i++) {
    for(j = 0; j < kNumDim; j++) {
      len = boxArray[i].ints[j].hi - boxArray[i].ints[j].lo;
			if(len >= maxLenBeforeScale) {
				halfLen = kRescaleFactor;
			} else {
				halfLen = (boxArray[i].ints[j].hi - boxArray[i].ints[j].lo)
				    				<< numPowersOf2;
			}
			mid = (boxArray[i].ints[j].hi + boxArray[i].ints[j].lo) >> 1; /* /2 */
      boxArray[i].ints[j].lo = mid - halfLen-1;
      boxArray[i].ints[j].hi = mid + halfLen+1;
    }
  }
}

//----------------------------------------------------------------------------------

void Tolerancer::
scaleDownBoxes_(boxesT *boxes, int numPowersOf2)
{
  int i, j;
  int numBoxes;
  int halflen, mid;
  boxT *boxArray;
  
  boxArray = GetBoxArray(boxes);
  numBoxes = GetNumBoxes(boxes);
  
  numPowersOf2++;  /* for half length */
  for(i = 0; i < numBoxes; i++) {
    for(j = 0; j < kNumDim; j++) {
      halflen = (boxArray[i].ints[j].hi - boxArray[i].ints[j].lo)
	>> numPowersOf2;
      mid = (boxArray[i].ints[j].hi + boxArray[i].ints[j].lo) >> 1; /* /2 */
      boxArray[i].ints[j].lo = mid - halflen-1;
      boxArray[i].ints[j].hi = mid + halflen+1;
    }
  }
}

//----------------------------------------------------------------------------------

void Tolerancer::
moveFailedVerticesToFront_(boxesT *boxes)
{
  boxT *boxArray;
  int numBoxes;
  boxT temp;
  int i, j;

  boxArray = GetBoxArray(boxes);
  numBoxes = GetNumBoxes(boxes);
  
  /* Swap failed ones to front */
  for(i = 0, j = 0; i < numBoxes; i++) {
    if(distances_[boxArray[i].num] < 0) {
      temp = boxArray[j];
      boxArray[j] = boxArray[i];
      boxArray[i] = temp;
      j++;
    }
  }
}


//----------------------------------------------------------------------------------

inline void Tolerancer::
computeSigns_(void)
{
	int i;
	double point[kNumDim];
	double side;

	for(i = 0; i < numPoints_; i++) {
		points_->getPoint(iwmap_[i], point);  
	  side = (evaluator_->*signCB_)(closestElements_[i], point);
		distances_[i] *= side;
	}
}

//----------------------------------------------------------------------------------

void Tolerancer::
computeClosestPtEdges_ (void)
{
  int i;
  EdgeCoord ec;
  DoublePoint point1;
  DoublePoint point2;
  double point[kNumDim];

  for (i = 0; i < numPoints_; i++)
  {
    if (fabs (distances_[i]) < 0.000001)
      continue;
	  points_->getPoint(iwmap_[i], point);  
    point1.load(point);
    point2.load(closestPts_[i]);
    ec.p1 = point1;
    ec.p2 = point2;
    closestPtEdges_->push (ec);
#ifdef __DEBUG__
    // Check that this is the closest point
    double newDist = 0.0;
    for (int j = 0; j < kNumDim; j++)
    {
      newDist += (closestPts_[i][j] - point[j]) * (closestPts_[i][j] - point[j]);
    }
    double newDistSqrt = sqrt(newDist);
    if (fabs (fabs (newDistSqrt) - fabs (distances_[i])) > 0.00000001)
      COMMENT ("Distances not equal!");
#endif
  }
}

//----------------------------------------------------------------------------------

CompDistanceEvaluator::
CompDistanceEvaluator (Complex * comp)
: TristModifier(comp->getTrist()), comp_(comp)
{

}

//----------------------------------------------------------------------------------

Bool CompDistanceEvaluator::
computeBBox (double bbox[2][3])
{
  RealPArray * rArray = comp_->getPArray();
  BitVector vUsed(rArray->size());
  BitVector& triBV = comp_->getFaceBV(COMPLETE_TRI);
  if(!triBV.getNumSet())
    return FALSE;
  double* minb = bbox[0], *maxb = bbox[1];
  double x[3];
  BitVectorIter iter(triBV);
  for(iter.first(); !iter.isDone(); iter.next())
  {
    OrTri ef(iter.curItem(), 0);
    for(int i=0; i<3; i++, ef = ef.enext())
    {
      VIndex v = Org_(ef);
      if(vUsed.test(v))
        continue;
      rArray->getPoint(v, x);
      if(!vUsed.getNumSet())
        vcopy(x, minb), vcopy(x, maxb);
      else
      {
        for(Dim d=0; d<3; d++)
          minb[d] = MIN(minb[d], x[d]), maxb[d] = MAX(maxb[d], x[d]);
      }
      vUsed.set(v);
    }
  }
  
  return TRUE;
}

//-----------------------------------------------------------------------------

Bool CompDistanceEvaluator::
computeTriangleBBox (int index, double bbox[2][3])
{
  int u, v, w;        // Vertex indices
  double p[3][3];    // Point coordinates
  //int j;

  // TODO:  Replace this with a generic bitvector test
  if (! comp_->inBoundary (Simplex(index, 2)))
    return FALSE;

  // TODO:  Replace this with a call to getTriangleVerts_
  comp_->getTrist()->triangle (OrTri(index, 0), u, v, w);
  RealPArray * rArray = comp_->getPArray();
  rArray->getPoint (u, p[0]);
  rArray->getPoint (v, p[1]);
  rArray->getPoint (w, p[2]);

  return computeTriangleBBox_ (p, bbox);
}

//-----------------------------------------------------------------------------

Bool CompDistanceEvaluator::
computeTriangleBBox_ (double v[3][kNumDim], double bbox[2][3])
{
  int j;

  for (j = 0; j < 3; j++)
    if (v[0][j] < v[1][j])
    {
      bbox[0][j] = v[0][j];
      bbox[1][j] = v[1][j];
    }
    else
    {
      bbox[1][j] = v[0][j];
      bbox[0][j] = v[1][j];
    }

  for (j = 0; j < 3; j++)
  {
    if (v[2][j] < bbox[0][j])
      bbox[0][j] = v[2][j];
    else if (v[2][j] > bbox[1][j])
      bbox[1][j] = v[2][j];
  }

  return TRUE;
}

//-----------------------------------------------------------------------------

double CompDistanceEvaluator::
computeDist (int index, double p[3], double * closestPt,
             double * closestPtBaryCoords)
{
  int v0i, v1i, v2i;            // Vertex indices
  double v[3][kNumDim];     // triangle vertices

  if (! comp_->inBoundary (Simplex(index, 2)))
    return FALSE;

  comp_->getTrist()->triangle (OrTri(index, 0), v0i, v1i, v2i);
  RealPArray *rArray = comp_->getPArray();
  rArray->getPoint (v0i, v[0]);
  rArray->getPoint (v1i, v[1]);
  rArray->getPoint (v2i, v[2]);

  return computeDist_ (v, p, closestPt, closestPtBaryCoords);
}

//-----------------------------------------------------------------------------

Bool CompDistanceEvaluator::
isValidTriangleIndex (int index)
{
  return (comp_->inBoundary (Simplex(index, 2)));
}

//-----------------------------------------------------------------------------

double CompDistanceEvaluator::
computeDist_ (double v[3][kNumDim], double p[3], double * closestPt,
              double * closestPtBaryCoords)
{
  // Compute the distance.

  double diff[kNumDim];
  double e0[kNumDim], e1[kNumDim];
  double tmp0, tmp1;
  double a, b, c, d, e, f, det;
  double s,t;
  double numer, denom;
  double dist;

  /* Portion added to Eberly's code to convert triangle description */
  AssignBinOpVector(e0, v[1], -, v[0]);
  AssignBinOpVector(e1, v[2], -, v[0]);

	/* Following Eberly's paper/code */
  AssignBinOpVector(diff, v[0], -, p);

  a = Dot(e0, e0);
  b = Dot(e0, e1);
  c = Dot(e1, e1);
  d = Dot(e0, diff);
  e = Dot(e1, diff);
  f = Dot(diff, diff);
  det = fabs(a*c - b*b);   /* a*c - b*b = |e0 x e1|^2 > 0 */

  /* Degeneracy Test for Triangle -- Return Min distance to edges */
  if(det <= 0.0) {
    int s2 = 0, t2 = 0;
    tmp0 = minPointLineSegmentDist_(p, v[0], e0, s2);
    tmp1 = minPointLineSegmentDist_(p, v[0], e1, t2);
    if (tmp0 < tmp1)
      dist = tmp0, s = s2, t = 0.0;
    else
      dist = tmp1, s = 0.0, t = t2;
  }
  else 
  {
    s = b*e - c*d;
    t = b*d - a*e;

    if (s+t <= det) {                 /* if under the line s+t */
      if (s < 0.0) {                  /* regions 3 & 4 */
        if (t < 0.0) {                /* region 4 */
          if (d < 0.0) {
            t = 0.0;
            if (-d >= a) {
              s = 1.0;
              dist = Dist(a+2.0*d+f);
            } else {
              s = -d / a;
              dist = Dist(d*s+f);
            }
          } 
          else {                    
            s = 0.0;
            if (e >= 0.0) {
              t = 0.0;
              dist = Dist(f);
            } 
            else if (-e >= c) 
            {
              t = 1.0;
              dist = Dist(c+2.0*e+f);
            } 
            else 
            {
              t = -e / c;
              dist = Dist(e*t+f);
            }
          }
        } 
        else 
        {                     /* region 3 */   
          s = 0.0;
          if (e >= 0.0) {
            t = 0.0;
            dist = Dist(f);
          } 
          else if (-e >= c) 
          {
            t = 1.0;
            dist = Dist(c+2.0*e+f);
          } 
          else 
          {
            t = -e / c; 
            dist = Dist(e*t+f);
          }
        }
      } 
      else if (t < 0) 
      {           /* region 5 */ 
        t = 0.0;
        if (d >= 0.0) { 
          s = 0.0;
          dist = Dist(f);
        }  
        else if (-d >= a) 
        {
          s = 1.0;
          dist = Dist(a+2.0*d+f);
        } 
        else 
        { 
          s = -d / a;
          dist = Dist(d*s+f);
        }
      } 
      else 
      {                     /* region 0 */
        tmp0 = 1.0 / det;
        s *= tmp0;
        t *= tmp0;
        dist = Dist(s*(a*s+b*t+2*d)+t*(b*s+c*t+2*e)+f);
      }
    } 
    else 
    {                      /* above line:  regions 2,6,1 */
      if (s < 0.0) 
      {              /* region 2 */
        tmp0 = b+d;
        tmp1 = c+e;
        if (tmp1 > tmp0) {
          numer = tmp1 - tmp0;
          denom = a-2.0*b+c;
          if(numer >= denom) {
            s = 1.0;
            t = 0.0;
            dist = (a+2.0*d+f);
          } 
          else 
          {
            s = numer/denom;
            t = 1.0-s;
            dist = Dist(s*(a*s+b*t+2*d)+t*(b*s+c*t+2*e)+f);
          }
        } 
        else 
        {
          s = 0.0;
          if (tmp1 <= 0.0) {
            t = 1.0;
            dist = Dist(c+2*e+f);
          } 
          else if (e >= 0.0) 
          {
            t = 0.0;
            dist = Dist(f);
          } else {
            t = -e / c;
            dist = Dist(e*t+f);
          }
        }
      } 
      else if (t < 0) 
      {        /* region 6 */
        tmp0 = b+e;
        tmp1 = a+d;
        if (tmp1 > tmp0) {
          numer = tmp1 - tmp0;
          denom = a-2.0*b+c;
          if(numer >= denom) {
            t = 1.0;
            s = 0.0;
            dist = Dist(c+2.0*e+f);
          } 
          else 
          {
            t = numer / denom;
            s = 1.0 - t;
            dist = Dist(s*(a*s+b*t+2*d)+t*(b*s+c*t+2*e)+f);
          }
        } 
        else 
        {
          t = 0.0;
          if (tmp1 <= 0.0) {
            s = 1.0;
            dist = Dist(a+2.0*d+f);
          } 
          else if (d >= 0.0) 
          {
            s = 0.0;
            dist = Dist(f);
          } 
          else 
          {
            s = -d / a;
            dist = Dist(d*s+f);
          }
        }
      } 
      else 
      {                   /* region 1 */
        numer = c+e-b-d;
        if (numer <= 0.0) {
          s = 0.0;
          t = 1.0;
          dist = Dist(c+2.0*e+f);
        } 
        else 
        {
	        denom = a-2.0*b+c;
	        if (numer >= denom) {
	          s = 1.0;
	          t = 0.0;
	          dist = Dist(a+2*d+f);
	        } else {
	          s = numer / denom;
	          t = 1.0 - s;
	          dist = Dist(s*(a*s+b*t+2*d)+t*(b*s+c*t+2*e)+f);
	        }
        }
      }
    }
  }

  if (closestPt != NULL)
  {
    int i;
    // Compute the closest point
    for (i = 0; i < kNumDim; i++)
    {
      closestPt[i] = v[0][i] + e0[i] * s + e1[i] * t;
    }
#ifdef __DEBUG__
    // Check that this is the closest point
    double newDist = 0.0;
    for (i = 0; i < kNumDim; i++)
    {
      newDist += (closestPt[i] - p[i]) * (closestPt[i] - p[i]);
    }
    if (fabs (newDist - dist) > 0.00000001)
      COMMENT ("Distances not equal!");
#endif
  }

  if (closestPtBaryCoords != NULL)
  {
     closestPtBaryCoords[0] = 1.0 - s - t;
     if (closestPtBaryCoords[0] < 0)
       closestPtBaryCoords[0] = 0.0;
     closestPtBaryCoords[1] = s;
     closestPtBaryCoords[2] = t;     
  }

  return dist;
}

//-----------------------------------------------------------------------------

// Returns 1 if on normal side (outside)
//        -1 if inside
//        0 if on surface
double CompDistanceEvaluator::
computeSide (int index, double p[3])
{
	int v0i, v1i, v2i;      // vertex indices
  double v[3][kNumDim];     // triangle vertices

  if (!comp_->inBoundary (Simplex(index, 2)))
    return FALSE;

  comp_->getTrist()->triangle (OrTri(index, 0), v0i, v1i, v2i);
  RealPArray *rArray = comp_->getPArray();
  rArray->getPoint (v0i, v[0]);
  rArray->getPoint (v1i, v[1]);
  rArray->getPoint (v2i, v[2]);

  return computeSide_ (v, p);
}

//-----------------------------------------------------------------------------

double CompDistanceEvaluator::
computeSide_ (double v[3][kNumDim], double p[3])
{
  int i;
  double det;

	// m = [1 v0]  = [v1-v0] 
	//     [1 v1]    [v2-v0]
	//     [1 v2]    [p -v0]
	//     [1 p ]
	// reducing the 4x4 matrix to a 3
	// by 3 one by subtracting first row from all
	// others
	for(i = 0; i < kNumDim; i++) {
		v[1][i] -= v[0][i];
		v[2][i] -= v[0][i];
		p[i]    -= v[0][i];
	}
	det = det3(v[1], v[2], p);
	if(det > 0.0) {
		return 1.0;
	} else if (det < 0.0) {
		return -1.0;
	}
	return 0.0;
}	

//-----------------------------------------------------------------------------

double CompDistanceEvaluator::
minPointLineSegmentDist_(double p[], double sb[], double sr[], double param)
{
  double diff[kNumDim];
  double t;
  double sr2;

  AssignBinOpVector(diff, p, -, sb);
  t = Dot(sr, diff);
  if(t <= 0.0) {       /* Degenerate Segment handled automatically */
    t = 0.0;
  } else {
    sr2 = Dot(sr, sr);
    if(t >= sr2) {
      t = 1.0;
      BinOpAndAssignVector(diff, -=, sr);
    } else {
      t /= sr2;
      diff[0] -= t*sr[0];
      diff[1] -= t*sr[1];
      diff[2] -= t*sr[2];
    }
  }
  param = t;
  return Dist(Dot(diff,diff));
}

