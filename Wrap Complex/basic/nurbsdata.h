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

#ifndef __RGI_NURBSDATA_H__
#define __RGI_NURBSDATA_H__

#include <iostream>

class BinInput;
class BinOutput;

class NurbSurfaceInfo;

/*---------------------------------------------------------------------------*/

// Error codes for NURBS operations

enum NurbsErrorCode
{
	SuccessfulOperation,
	CantMergeNonArrayStructure,
	CantMergeSharedBoundaryWithCusp,
	CantMergePartialCorner,
	CantMergeAlongDegreeThreeCorner,
	CantMergeAlreadyMergedPatches,
	MaxNurbsErrorCode
};

enum ContinuityType
{
	C0Continuity = 0,
	TangentContinuity = 1,
	CurvatureContinuity = 2
};

/*---------------------------------------------------------------------------*/

// Structures for converting NLIB definitions into our
// definition

// NLib Surface:
// {
//  CNET * net; =
//    CPOINT ** points; = { REAL (= double) x, y, z, w };
//    INDEX (= INTEGER = long) m, n;
//  DEGREE ( = short) p, q;
//  KNOTVECTOR =  knu, knv;
//    = INDEX (long) m,
//        REAL (double) * U;
// }

class NlibPoint
{
public:
  double x, y, z;
  NlibPoint (double _x = 0, double _y = 0, double _z = 0): x(_x), y(_y), z(_z) { }
};

class NlibCPoint
{
public:
  double x, y, z, w;
  NlibCPoint (double _x = 0, double _y = 0, double _z = 0, double _w = 0)
    : x(_x), y(_y), z(_z), w(_w) { }
	int binInit (BinInput& bi);
	void binDump (BinOutput& bo);
};

class NlibKnotvector
{
public:
  long m;
  double * u;
  NlibKnotvector (void) : m(0), u(NULL) { }
  ~NlibKnotvector (void) { delete [] u; }
};

class NlibCPolygon
{
public:
  long n;
  NlibCPoint * points;
  NlibCPolygon (void) : n(0), points(NULL) { }
  ~NlibCPolygon (void) { delete [] points; }
};

class NlibCurve
{
public:
  NlibCPolygon * pol; // Control points
  short p; // degree in u directions
  NlibKnotvector *knu;
  void * SG; // this is nlib-related allocation pointer
  NlibCurve (void);
  ~NlibCurve (void);
};

class NlibCNet
{
public:
  long m, n;
    // m+1 x n+1 Control points in this patch, in the
    // interval [0..m] x [0..n]
  NlibCPoint ** points;
  NlibCNet (void);
  ~NlibCNet (void);
};

class  NlibSurface
{
public:
  NlibCNet * net; // Control points
  short p, q; // degrees of u and v directions
  NlibKnotvector *knu, *knv;
  void * SG; // this is nlib-related allocation pointer
  NlibSurface (void);
  NlibSurface (NlibSurface& other);
  ~NlibSurface (void);
  void evalNurbs (double u, double v, NlibPoint& p, NlibPoint& n);
  void evalNurbs (NurbSurfaceInfo & surfInfo, int doCurvature);
	int binInit (BinInput& bi);
	void binDump (BinOutput& bo);
};

/*---------------------------------------------------------------------------*/

ostream&  operator<< (ostream& s, const NlibPoint& p);
ostream&  operator<< (ostream& s, const NlibCPoint& p);
ostream&  operator<< (ostream& s, const NlibKnotvector& vector);
ostream&  operator<< (ostream& s, const NlibCPolygon& net);
ostream&  operator<< (ostream& s, const NlibCurve& surf);
ostream&  operator<< (ostream& s, const NlibCNet& net);
ostream&  operator<< (ostream& s, const NlibSurface& surf);

/*---------------------------------------------------------------------------*/

class NurbSurfaceInfo
{
public:
  NurbSurfaceInfo (void);
  ~NurbSurfaceInfo (void);

  void setResolution (int uRes, int vRes);
    //- Sets the current resolution factors, and resizes
    //- the arrays as needed.

  int curURes, curVRes;
  int allocURes, allocVRes;
  NlibPoint ** points;  // [0..patchRes][0..patchRes]
  NlibPoint ** normals;  // [0..patchRes][0..patchRes]
  double ** curvature;  // [0..patchRes][0..patchRes]
  NlibPoint ** prinCurv1;  // [0..patchRes][0..patchRes]
  NlibPoint ** prinCurv2;  // [0..patchRes][0..patchRes]
  double ** prinCurvK1;  // [0..patchRes][0..patchRes]
  double ** prinCurvK2;  // [0..patchRes][0..patchRes]

private:
  void allocate (void);
    //- Allocate memory for the (allocURes, allocVRes) values.
  void deallocate (void);
    //- Deallocate all memory.
};

/*---------------------------------------------------------------------------*/

class NurbContinuityInfo
{
public:
	NurbContinuityInfo (void);
	~NurbContinuityInfo (void);
	NurbContinuityInfo (NurbContinuityInfo & info);
	void initC0 (void);
	void initG1 (int _numCornerDegree, int _numNonG1Levels, double * _nonG1Levels = NULL);
		//- Initialize the structure with the corresponding variables
	double minAngle;
		//-  Minimum dihedral angle
	double maxAngle;
		//-  Maximum dihedral angle
	double avgAngle;
		//-  Average dihedral angle
	double stdDevAngle;
		//-  Standard deviation of dihedral angles
	int numAngles;
		//-  Number of angle evaluations actually performed. This may
		//-  be different from numG1Checks below due to sharp boundaries.
	int numNonC0;
		//- Number of non-C0 points.
	int numNonG1;
		//- Total number of non-G1 points
	int numG1Checks;
		//- Number of non-G1 checks performed
	int numNonG1Corner;
		//- Number of non-G1 points that occur near a corner
	int numNonG1Boundary;
		//- Number of non-G1 points that occur on a boundary
	int numCornerDegree;
		//- Maximum corner degree handled directly in the array below
	int *numNonG1CornerDegree;
		//- Array in range [0..numCornerDegree-1].  Position i contains the number
		//- of non-G1 points near a degree i corner, with the last position containing
		//- all higher degrees.
	int numNonG1Levels;
		//- Number of levels for dividing the statistics on dihedral angles.
	double *nonG1Levels; 
		//- Levels for statistics on dihedral angles.  Array with range [0..numNonG1Levels-1].  
	int *numNonG1ByLevel;
		//- Array with range [0..numNonG1Levels-1].  
};

ostream&  operator<< (ostream& s, const NurbContinuityInfo& p);

/*---------------------------------------------------------------------------*/

#endif // __NLIBDATA_H__
