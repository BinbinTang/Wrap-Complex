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

#include <basic/basic.h>
#include <basic/binio.h>
#include <basic/nurbsdata.h>


extern void deallocateNlibStack (void **);
extern void evalNurbs (NlibSurface& surf, double u, double v, NlibPoint& p, NlibPoint& n);
extern void evalNurbs (NlibSurface& surf, NurbSurfaceInfo & surfInfo, int doCurvature);
extern void allocateNlibStack (void **);
extern void allocateSurfaceMemory (void **);
extern void allocateSurfaceMemory (NlibSurface& surf, int n, int m, int p, int q);

ostream&  operator<< (ostream& s, const NlibPoint& p)
{
  s << "(" << p.x << ", " << p.y << ", " << p.z << ")" << endl;
  return s;
}

ostream&  operator<< (ostream& s, const NlibCPoint& p)
{
  s << "(" << p.x << ", " << p.y << ", " << p.z << ";" << p.w << ")" << endl;
  return s;
}

ostream&  operator<< (ostream& s, const NlibKnotvector& vector)
{
  s << "\t\tSize " << vector.m+1 << endl;
  for (int i = 0; i <= vector.m; i++)
  {
    if (i % 10 == 0)
      s << endl << "\t\t\t";
    s << vector.u[i] << " ";
  }
  s << endl;
  return s;
}

ostream&  operator<< (ostream& s, const NlibCPolygon& pol)
{
  s << "\t\tSize " << pol.n << endl;
  for (int i = 0; i <= pol.n; i++)
      s << "\t\t\t" << pol.points[i];
  s << endl;
  return s;
}

ostream&  operator<< (ostream& s, const NlibCurve& cur)
{
  s << "\tDegree: " << cur.p << endl;
  if (cur.knu)
  {
    s << "\tU KnotVector: " << endl;
    s << *cur.knu;
  }
  s << "\tControl Points: " << endl;
  s << *cur.pol;

  return s;
}

ostream&  operator<< (ostream& s, const NlibCNet& net)
{
  s << "\t\tSize " << net.m+1 << " x " << net.n+1 << endl;
  for (int i = 0; i <= net.m; i++)
  {
    s << "\t\t\tU [" << i << "]:" << endl;
    for (int j = 0; j <= net.n; j++)
      s << "\t\t\t\t" << net.points[i][j];
  }
  s << endl;
  return s;
}

ostream&  operator<< (ostream& s, const NlibSurface& surf)
{
  s << "\tU degree: " << surf.p << ", V degree: " << surf.q << endl;
  if (surf.knu)
  {
    s << "\tU KnotVector: " << endl;
    s << *surf.knu;
  }
  if (surf.knv)
  {
    s << "\tV KnotVector: " << endl;
    s << *surf.knv;
  }
  s << "\tControl Points: " << endl;
  s << *surf.net;

  return s;
}

//--------------------------------------------------------------

int NlibCPoint::
binInit (BinInput &bi)
{
	x = bi.readDouble ();
	y = bi.readDouble ();
	z = bi.readDouble ();
	w = bi.readDouble ();
	return TRUE;
}

void NlibCPoint::
binDump (BinOutput &bo)
{
	bo.writeDouble (x);
	bo.writeDouble (y);
	bo.writeDouble (z);
	bo.writeDouble (w);
}

NlibCNet::
NlibCNet (void)
: m(0), n(0), points(NULL)
{
  
}

NlibCNet::
~NlibCNet (void)
{
  
}

NlibCurve::
NlibCurve (void) : pol(NULL), p(-1), knu(NULL), SG(NULL)
{ 
  ::allocateNlibStack(&SG);
}

NlibCurve::
~NlibCurve (void)
{
  ::deallocateNlibStack(&SG);
}

NlibSurface::
NlibSurface (void)
: net(NULL), p(-1), q(-1), knu(NULL), knv(NULL), SG(NULL)
{
  ::allocateNlibStack(&SG);
}

NlibSurface::
NlibSurface (NlibSurface& other)
{
  ::allocateNlibStack(&SG);
	::allocateSurfaceMemory (*this, other.net->m, other.net->n, other.p, other.q);
	int i, j;
	// U knot vector
	for (i = 0; i <= other.knu->m; i++)
		knu->u[i] = other.knu->u[i];
	// V knot vector
	for (j = 0; j <= other.knv->m; j++)
		knv->u[j] = other.knv->u[j];
	// Control points
	for (i = 0; i <= other.net->m; i++)
		for (j = 0; j <= other.net->n; j++)
			net->points[i][j] = other.net->points[i][j];
	// Let's see it!
	// cout << "Copy constructor for new patch: " << endl << *this << endl;
}

NlibSurface::
~NlibSurface (void)
{
  ::deallocateNlibStack(&SG);
}

void NlibSurface::
evalNurbs (double u, double v, NlibPoint& p, NlibPoint& n)
{
  ::evalNurbs(*this, u, v, p, n);
}

void NlibSurface::
evalNurbs (NurbSurfaceInfo & surfInfo, int doCurvature)
{
  ::evalNurbs(*this, surfInfo, doCurvature);
}

#define NLIBSURFACE_MAGIC 0x55474714
#define NLIBSURFACE_VERSION 1

int NlibSurface::
binInit (BinInput &bi)
{
	Magic magic (NLIBSURFACE_MAGIC, NLIBSURFACE_VERSION);
	if (!magic.binInit (bi)) {
		return FALSE;
	}

	p = bi.readInt();
	q = bi.readInt();
	long m = bi.readInt();
	long n = bi.readInt();
	allocateSurfaceMemory (*this, m, n, p, q);
	bi.readDoubles (knu->u, knu->m+1);
	bi.readDoubles (knv->u, knv->m+1);
	for (int i=0; i<=m; i++) {
		for (int j=0; j<=n; j++) {
			net->points[i][j].binInit (bi);
		}
	}
	return TRUE;
}

void NlibSurface::
binDump (BinOutput &bo)
{
	Magic magic (NLIBSURFACE_MAGIC, NLIBSURFACE_VERSION);
	magic.binDump (bo);

	bo.writeInt (p);
	bo.writeInt (q);
	bo.writeInt (net->m);
	bo.writeInt (net->n);
	bo.writeDoubles (knu->u, knu->m+1);
	bo.writeDoubles (knv->u, knv->m+1);
	for (int i=0; i<=net->m; i++) {
		for (int j=0; j<=net->n; j++) {
			net->points[i][j].binDump (bo);
		}
	}
}

//---------------------------------------------------------------------------------

NurbSurfaceInfo::
NurbSurfaceInfo (void)
{
	points = NULL;
	normals = NULL;
  curvature = NULL;
  prinCurv1 = NULL;
  prinCurv2 = NULL;
  prinCurvK1 = NULL;
  prinCurvK2 = NULL;
  curURes = curVRes = 0;
  allocURes = allocVRes = 0;
}


void NurbSurfaceInfo::
allocate (void)
{
  int i;
	points = new NlibPoint* [allocURes+1];
	normals = new NlibPoint* [allocURes+1];
  curvature = new double* [allocURes+1];
  prinCurv1 = new NlibPoint* [allocURes+1];
  prinCurv2 = new NlibPoint* [allocURes+1];
  prinCurvK1 = new double* [allocURes+1];
  prinCurvK2 = new double* [allocURes+1];
	for(i = 0; i < allocURes+1; i++)
	{
		points[i] = new NlibPoint [allocVRes+1];
		normals[i] = new NlibPoint [allocVRes+1];
		curvature[i] = new double [allocVRes+1];
		prinCurv1[i] = new NlibPoint [allocVRes+1];
		prinCurv2[i] = new NlibPoint [allocVRes+1];
		prinCurvK1[i] = new double [allocVRes+1];
		prinCurvK2[i] = new double [allocVRes+1];
	}
}

void NurbSurfaceInfo::
setResolution (int uRes, int vRes)
{
  int doReallocate = FALSE;
  curURes = uRes;
  curVRes = vRes;
	int newAllocURes = allocURes;
	int newAllocVRes = allocVRes;
  if (allocURes < uRes)
  {
    newAllocURes = uRes;
    doReallocate = TRUE;
  }
  if (allocVRes < vRes)
  {
    newAllocVRes = vRes;
    doReallocate = TRUE;
  }
  if (doReallocate)
  {
    deallocate ();
		allocURes = newAllocURes;
		allocVRes = newAllocVRes;
    allocate ();
  }
}

NurbSurfaceInfo::
~NurbSurfaceInfo (void)
{
  deallocate ();
}

void NurbSurfaceInfo::
deallocate (void)
{
  int i;
  if (points == NULL)
    return;

	for(i=0; i < allocURes+1; i++)
	{
		delete [] points[i];
		delete [] normals[i];
		delete [] curvature[i];
		delete [] prinCurv1[i];
		delete [] prinCurv2[i];
		delete [] prinCurvK1[i];
		delete [] prinCurvK2[i];
	}
	delete [] points;
	delete [] normals;
	delete [] curvature;
	delete [] prinCurv1;
	delete [] prinCurv2;
	delete [] prinCurvK1;
	delete [] prinCurvK2;

	points = NULL;
	normals = NULL;
  curvature = NULL;
  prinCurv1 = NULL;
  prinCurv2 = NULL;
  prinCurvK1 = NULL;
  prinCurvK2 = NULL;
}

//---------------------------------------------------------------------------------

NurbContinuityInfo::
NurbContinuityInfo (void)
{
	numNonG1CornerDegree = NULL;
	nonG1Levels = NULL;
	numNonG1ByLevel = NULL;
}

//---------------------------------------------------------------------------------

NurbContinuityInfo::
NurbContinuityInfo (NurbContinuityInfo & info)
{
	int i;

	numNonC0 = info.numNonC0;
	numNonG1 = info.numNonG1;
	numNonG1Corner = info.numNonG1Corner;
	numNonG1Boundary = info.numNonG1Boundary;
	numG1Checks = info.numG1Checks;
	minAngle = info.minAngle;
	maxAngle = info.maxAngle;
	avgAngle = info.avgAngle;
	stdDevAngle = info.stdDevAngle;
	numAngles = info.numAngles;

	numCornerDegree = info.numCornerDegree;
	numNonG1CornerDegree = new int[numCornerDegree];
	for (i = 0; i < numCornerDegree; i++)
		numNonG1CornerDegree[i] = info.numNonG1CornerDegree[i];
	numNonG1Levels = info.numNonG1Levels;
	nonG1Levels	= new double[numNonG1Levels];
	numNonG1ByLevel = new int[numNonG1Levels];
	for (i = 0; i < numNonG1Levels; i++)
	{
		nonG1Levels[i] = info.nonG1Levels[i];
		numNonG1ByLevel[i] = info.numNonG1ByLevel[i];
	}

}

//---------------------------------------------------------------------------------

NurbContinuityInfo::
~NurbContinuityInfo (void)
{
	delete [] numNonG1CornerDegree;
	delete [] nonG1Levels;
	delete [] numNonG1ByLevel;
}

//---------------------------------------------------------------------------------

void NurbContinuityInfo::
initC0 (void)
{
	numNonC0 = 0;
}

//---------------------------------------------------------------------------------

void NurbContinuityInfo::
initG1 (int _numCornerDegree, int _numNonG1Levels, double * _nonG1Levels)
{
	int i;

	numNonG1 = 0;
	numNonG1Corner = 0;
	numNonG1Boundary = 0;
	numG1Checks = 0;
	minAngle = 360.0;
	maxAngle = 0.0;
	avgAngle = 0.0;
	stdDevAngle = 0.0;
	numAngles = 0;

	numCornerDegree = _numCornerDegree;
	delete [] numNonG1CornerDegree;
	numNonG1CornerDegree = new int[numCornerDegree];
	for (i = 0; i < numCornerDegree; i++)
		numNonG1CornerDegree[i] = 0;
	numNonG1Levels = _numNonG1Levels;
	delete [] nonG1Levels;
	nonG1Levels	= new double[numNonG1Levels];
	delete [] numNonG1ByLevel;
	numNonG1ByLevel = new int[numNonG1Levels];
	for (i = 0; i < numNonG1Levels; i++)
	{
		if (_nonG1Levels != NULL)
			nonG1Levels[i] = _nonG1Levels[i];
		else
			nonG1Levels[i] = 0;
		numNonG1ByLevel[i] = 0;
	}

}

//---------------------------------------------------------------------------------

ostream&  operator<< (ostream& s, const NurbContinuityInfo& info)
{
	int i;

	s << "Continuity Check:" << endl;
  s << "\tNumber of non-C0 points = " << info.numNonC0 << endl;

	s << "\tNumber of non-G1 points = " << info.numNonG1 << 
	" out of " << info.numG1Checks << " performed:" << endl;
	s << "\tNon-G1 Corner Points = " << info.numNonG1Corner << endl;
	for (i = 1; i < 10; i++)
		s << "\t\tDegree " << i << " = " << info.numNonG1CornerDegree[i]
		<< " (" << info.numNonG1CornerDegree[i] / (i * 3) << ")." << endl;
	s << "\tNon-G1 Boundary Points = " << info.numNonG1Boundary << endl;
	s << endl << "\tMinimum Angle = " << info.minAngle << endl;
	s << endl << "\tMaximum Angle = " << info.maxAngle << endl;
	s << endl << "\tAverage Angle = " << info.avgAngle << endl;
	s << endl << "\tNon G1 by level:" << endl;
	for (i = 0; i < info.numNonG1Levels; i++)
		if (i == info.numNonG1Levels-1)
			s << "\t\t[" << info.nonG1Levels[i] << ", inf) : " 
				<< info.numNonG1ByLevel[i] << "." << endl;
		else
			s << "\t\t[" << info.nonG1Levels[i] << ", " << info.nonG1Levels[i+1] 
				<< ") : " << info.numNonG1ByLevel[i] << "." << endl;
	s << endl << endl;

	return s;
}
