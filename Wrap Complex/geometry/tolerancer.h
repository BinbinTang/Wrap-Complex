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

#ifndef __RGI_TOLERANCER_H__
#define __RGI_TOLERANCER_H__

#include <basic/callbackobject.h>
#include <basic/rarray.h>
#include <geometry/geomutil.h>
#include <geometry/boxes.h>

#ifdef __DEBUG__
#define DO_PRINT_OUTPUT 1
#else
#define DO_PRINT_OUTPUT 0
#endif

// Callbacks used by Tolerancer
typedef int (RgiObject::*ComputeBBoxCB) (double bbox[2][3]);
typedef int (RgiObject::*ComputeElemBBoxCB) (int index, double bbox[2][3]);
typedef double (RgiObject::*ComputeDistCB) (int index, double p[3], 
                                            double * closestPt, double * closestPtBaryCoord);
typedef double (RgiObject::*ComputeSideCB) (int index, double p[3]);
typedef Bool (RgiObject::*ValidIndexCB) (int index);

class WrapPointsI;
class EdgeCoordStack;

class Tolerancer : public ProgressCallbackObject
{
public:
  Tolerancer (int maxElement,BitVector* tbv);
  ~Tolerancer (void);

  enum PointType { FloatPoints, DoublePoints };
  void setPointData (RealPArray *pointsArray,BitVector* pointsbv);

  void setCallbackFuncs (RgiObject * evaluator, 
												 ComputeBBoxCB bboxCB, 
												 ComputeElemBBoxCB elemBBoxCB, 
												 ComputeDistCB distCB,
												 ComputeSideCB signCB,
                         ValidIndexCB isValidCB);
  void setComputeClosestPts (int onOff) 
      { doComputeClosestPts_ = onOff; }
  void setComputeClosestPtBaryCoords (int onOff) 
      { doComputeClosestPtBaryCoords_ = onOff; }
	Bool computeTolerance (void);
  int getNumPoints (void) { return pbv_->getNumSet(); }
	Bool getDistances(double distances[]);
	Bool getSignedDistances(double distances[]);
	Bool getDistanceRGBs(double rgb[][3]);
	Bool getDistanceRGBs(double rgb[][3], double minVal, double maxVal, double accept = 0.0);
  Bool getStatistics (double &maxDist, double &avgDist, double &stdDev, int printOutput = DO_PRINT_OUTPUT);
  
  double* getDistanceArray (void) { return distances_; }
  int * getClosestElements (void) { return closestElements_; }
  double ** getClosestPtBaryCoords (void) { return closestPtBaryCoords_; }
  EdgeCoordStack * getClosestPts (void) { return closestPtEdges_; }

	void computeDistanceSquaredCB (int pointNum, int elemNum);

private:

	BitVector* tbv_;
	BitVector* pbv_;

	int* wimap_; // original index map to new index (wpi -> internal index)
	int* iwmap_; // the other way round (internal -> wpi)

	// Have we computed distances
	Bool computedDistances_;
  Bool doComputeClosestPts_;
  Bool doComputeClosestPtBaryCoords_;
  
	// Elements
	int maxElement_;
  RgiObject * evaluator_;
  ComputeBBoxCB bboxCB_;
  ComputeElemBBoxCB elemBBoxCB_;
  ComputeDistCB distCB_;
	ComputeSideCB signCB_;
  ValidIndexCB isValidCB_;

  // Points
  RealPArray* points_;
	int numPoints_;

	// Output (computed values)
	double *distances_;
	int *closestElements_;
  double **closestPts_;
  double **closestPtBaryCoords_;
  EdgeCoordStack * closestPtEdges_;

	// Helper methods
	void putBoundingBoxAroundPoints_(double box[2][kNumDim]);
	double putBoundingCubeAroundBoxes_(double box1[2][kNumDim], 
                                     double box2[2][kNumDim], 
																	   double bbox[2][kNumDim],
																		 double &diagonal); 
  void putEmptyBoxesAroundPoints_(boxesT *boxes, double factor, 
		                              double mins[kNumDim]);
	void putBoundingCubesAroundElements_(boxesT *boxes, double factor, 
																	     double mins[kNumDim]);
	inline double findLengths_(double box[2][kNumDim], double lengths[kNumDim]);
	void findDistanceFailedPoints_(boxesT *pointsBoxes, boxesT *surfaceBoxes,
																 int numWithoutDist);
	inline int countNegativeValues_(double array[], int num);
	void scaleUpBoxes_(boxesT *boxes, int numPowersOf2);
  void scaleDownBoxes_(boxesT *boxes, int numPowersOf2);
	void moveFailedVerticesToFront_(boxesT *boxes);
	void putDistanceBoxesAroundPoints_(boxesT *boxes, double factor, double mins[kNumDim]);
	void putBoundingBoxesAroundElements_(boxesT *boxes, double factor, double mins[kNumDim]);
	void computeColorSpace(const double intensity, double rgb[3], const double accept, 
						   const double minVal, const double maxVal);
	inline void computeHotColormapRGB_(double intensity, double rgb[3]);
	inline void computeBlueRedSignedRGB_(double intensity, double rgb[3]);
	inline double findAbsMax_(double array[], int num);
	inline void computeSigns_(void);
  void computeClosestPtEdges_ (void);
};

class CompDistanceEvaluator : public TristModifier
{
public:
  CompDistanceEvaluator (Complex * comp);
  ~CompDistanceEvaluator (void) { }
  Bool computeBBox (double bbox[2][3]);
  Bool computeTriangleBBox (int index, double bbox[2][3]);
	double computeSide (int index, double p[3]);
  double computeDist (int index, double p[3], double * closestPt,
        double * closestPtBaryCoords);
  Bool isValidTriangleIndex (int index);
protected:
  Complex * comp_;
  Bool computeTriangleBBox_ (double v[3][kNumDim], double bbox[2][3]);
	double computeSide_ (double v[3][kNumDim], double p[3]);
  double computeDist_ (double v[3][kNumDim], double p[3], double * closestPt,
        double * closestPtBaryCoords);
  double minPointLineSegmentDist_(double p[kNumDim], double sb[kNumDim], double sr[kNumDim], double param);
};

#endif
