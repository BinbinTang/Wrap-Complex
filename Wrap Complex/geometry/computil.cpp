/*
// Copyright 1996-97. Raindrop Geomagic, Inc.
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
*/

#include <basic/miscmath.h>
#include <basic/rectsel.h>
#include <basic/comment.h>

#include <geometry/computil.h>

Complex_SelectUtil::
Complex_SelectUtil (Complex * comp)
: TristModifier (comp->getTrist()), comp_(comp)
{

}


Bool Complex_SelectUtil::
getPointOnEdge(OrTri ef, RectSelection& rectSel, double dx[3])
{
  double ax[3], bx[3];
  comp_->getCoords(Org_(ef), ax);
  comp_->getCoords(Dest_(ef), bx);
  double axm[3], bxm[3], mxm[3];
  rectSel.transform(ax, axm); axm[2] = 0; // a in mouse coords
  rectSel.transform(bx, bxm); bxm[2] = 0; // b in mouse coords
  mxm[0] = rectSel.x1(); mxm[1] = rectSel.y1(); mxm[2] = 0; // mouse
  // transform origin to a
  vsub(bxm, axm);
  vsub(mxm, axm);
  // find k so ad = k*ab
  double bl = vlength2(bxm);
  if (!bl) bl = 1.0;
  double k = vdot(mxm, bxm)/bl;
  if (k > 0.95) k = 0.95;
  if (k < 0.05) k = 0.05;
  // apply k to untransformed coords
  vadd (vmul(ax, 1-k), vmul(bx, k), dx);
  return TRUE;
}

//-----------------------------------------------------------------

Bool Complex_SelectUtil::
getPointOnTri(OrTri ef, RectSelection& rectSel, double dx[3], Bool exactPointOnTri)
{
  double amountInside;
  double ax[3], bx[3], cx[3];
  int a, b, c;
  double beta = 0.33333;
  double gamma = 0.33333;
  Triangle_(ef, a, b, c);

  if (exactPointOnTri)
    amountInside = 0.0;
  else
    amountInside = 0.05;
  comp_->getCoords(a, ax);
  comp_->getCoords(b, bx);
  comp_->getCoords(c, cx);
  double axm[3], bxm[3], cxm[3], mxm[3];
  rectSel.transform(ax, axm); axm[2] = 0; // a in mouse coords
  rectSel.transform(bx, bxm); bxm[2] = 0; // b in mouse coords
  rectSel.transform(cx, cxm); cxm[2] = 0; // c in mouse coords
  mxm[0] = rectSel.x1(); mxm[1] = rectSel.y1(); mxm[2] = 0; // mouse
  // transform origin to a
  vsub(bxm, axm);
  vsub(cxm, axm);
  vsub(mxm, axm);
  double det = (bxm[0]*cxm[1]-bxm[1]*cxm[0]);
  if (det)
  {
	  beta = (mxm[0]*cxm[1]-mxm[1]*cxm[0])/det;
	  gamma = (bxm[0]*mxm[1]-bxm[1]*mxm[0])/det;
	  if (beta < amountInside) beta = amountInside;
	  if (gamma < amountInside) gamma = amountInside;
	  if (beta > (1.0 - amountInside)) beta = 1.0 - amountInside;
	  if (gamma > (1.0 - amountInside)) gamma = 1.0 - amountInside;
	  if ((beta + gamma) > (1.0 - amountInside))
	  {
      double tmp = (1.0 - amountInside) / (beta + gamma);
	    beta *= tmp;
	    gamma *= tmp;
	  }
  }

  vmul(ax, 1 - (beta + gamma), dx);
  vadd(dx, vmul(bx, beta));
  vadd(dx, vmul(cx, gamma));
  return TRUE;
}

//-----------------------------------------------------------------

OrTri Complex_SelectUtil::
getClosestVertexOrTri (RectSelection& rectSel, int fInd)
{
  int i;
  int v[3];
  double dist[3];
  double c[3], p[3][3], vx[3];
  OrTri ef(fInd, 0);
  Triangle_(ef, v[0], v[1], v[2]);

  vx[0] = rectSel.x1();
  vx[1] = rectSel.y1();
  vx[2] = 0;
  
  for(i=0; i<3; i++)
  {
    comp_->getCoords(v[i], c);
    rectSel.transform(c, p[i]);
	  p[i][2] = 0;
	  dist[i] = vdist2(p[i], vx);
  }

  double mindist = MIN(MIN(dist[0], dist[1]), dist[2]);

  if (mindist == dist[1])
    ef = Enext_ (ef);
  else if (mindist == dist[2])
    ef = Enext2_ (ef);

  // Determine orientation
  double det =   p[0][0] * (p[1][1] - p[2][1])
                - p[0][1] * (p[1][0] - p[2][0])
                + p[1][0] * p[2][1] - p[1][1] * p[2][0];

  // This used to be (det < 0)
  if (det > 0)
    ef = Enext_ (Sym_ (ef));

//  if (! getCurComplex_()->facesOutward (Ksimplex (ef, 2)))
//	  COMMENT("Not boundary triangle!!!");

  return ef;
}

//-----------------------------------------------------------------

OrTri Complex_SelectUtil::
getClosestEdgeOrTri (RectSelection& rectSel, int fInd)
{
  int i;
  int v[3];
  double dist[3];
  double c[3], p[3][3], vx[3];
  OrTri ef(fInd, 0);
  Triangle_(ef, v[0], v[1], v[2]);

  vx[0] = rectSel.x1();
  vx[1] = rectSel.y1();
  vx[2] = 0;
  
  for(i=0; i<3; i++)
  {
    comp_->getCoords(v[i], c);
    rectSel.transform(c, p[i]);
	  p[i][2] = 0; // do not use z-coord;
  }

  for(i=0; i<3; i++)
  {
    dist[i] = pointToLineDist2(vx, p[i], p[(i+1)%3]);
  }

  double mindist = MIN(MIN(dist[0], dist[1]), dist[2]);

  if (mindist == dist[1])
    ef = Enext_ (ef);
  else if (mindist == dist[2])
    ef = Enext2_ (ef);

  // Determine orientation

  double det =   p[0][0] * (p[1][1] - p[2][1])
                - p[0][1] * (p[1][0] - p[2][0])
                + p[1][0] * p[2][1] - p[1][1] * p[2][0];

  // This used to be (det < 0)
  if (det > 0)
    ef = Sym_(ef);

  return ef;
}

//-----------------------------------------------------------------

OrTri Complex_SelectUtil::
getOrTri (RectSelection& rectSel, int fInd)
{
  int i;
  int v[3];
  double c[3], p[3][3];
  OrTri ef(fInd, 0);
  Triangle_(ef, v[0], v[1], v[2]);

  for(i=0; i<3; i++)
  {
    comp_->getCoords(v[i], c);
    rectSel.transform(c, p[i]);
	p[i][2] = 0;
  }

  // Determine orientation
  double det =   p[0][0] * (p[1][1] - p[2][1])
                - p[0][1] * (p[1][0] - p[2][0])
                + p[1][0] * p[2][1] - p[1][1] * p[2][0];

  // This used to be (det < 0)
  if (det > 0)
    ef = Enext_ (Sym_ (ef));

//  if (! getCurComplex_()->facesOutward (Ksimplex (ef, 2)))
//	  cout << "Not boundary triangle!!!" << endl;

  return ef;
}

//-----------------------------------------------------------------

OrTriStack * Complex_SelectUtil::
getOrTri (RectSelection& rectSel, IndexStack & iStack)
{
	OrTriStack * oStack = new OrTriStack (iStack.size ());
  OrTri ef;
	IStackIter<Index> iter (iStack);
	for (iter.first (); ! iter.isDone (); iter.next ())
		oStack->push (ef = getOrTri (rectSel, iter.curItem ()));

	return oStack;
}

OrTriStack * Complex_SelectUtil::
getOrTri (RectSelection& rectSel, BitVector & bv)
{
	OrTriStack * oStack = new OrTriStack (bv.getNumSet ());
    OrTri ef;
	BitVectorIter iter(bv);
	for (iter.first (); ! iter.isDone (); iter.next ())
		oStack->push (ef = getOrTri (rectSel, iter.curItem ()));

	return oStack;
}

//-----------------------------------------------------------------

// group functions - we pass bitvector of marked triangle indices,
// and return bitvector of marked vertex indices and ortri stack
// representing each of them.
// efStack maybe not empty already
void Complex_SelectUtil::
markRectanglePoints (RectSelection& rectSel, BitVector& triBV,
					 BitVector& vertBV, OrTriStack * efStack)
{
  BVTag vTag(comp_->getBVPool());
  VIndex v;
  FIndex f;
  // set all vertices first
  BitVectorIter triIter(triBV);
  for (triIter.first(); !triIter.isDone(); triIter.next())
  {
	
	f = triIter.curItem();
	if (!comp_->inComplex(Simplex(f, 2)))
	  continue;
	VIndex a, b, c;
    Triangle_(OrTri(f, 0), a, b, c);
	vertBV.set(a); vertBV.set(b); vertBV.set(c);
  }

  // make sure all vertices are inside of the rectangle
  BitVectorIter vIter(vertBV);
  double vx[3];
  RealPArray * rArray = comp_->getPArray();
  for (vIter.first(); !vIter.isDone(); vIter.next())
  {
    v = vIter.curItem();
	rArray->getPoint(v, vx);
	if (!rectSel.inside(vx)) vertBV.clear(v);
  }

  // now set ef representative
  if (efStack)
  {
    for (triIter.first(); !triIter.isDone(); triIter.next())
	{
	  f = triIter.curItem();
	  if (!comp_->inComplex(Simplex(f, 2)))
	    continue;
	  OrTri ef(f, 0);
	  for(int i=0; i<3; i++, ef = Enext_(ef))
	  {
        v = Org_(ef);
	    if (!vertBV.test(v) || vTag.test(v))
		  continue;
	    vTag.set(v);
	    efStack->push(ef);
	  }
	}
  }

}


Bool Complex_SelectUtil::
selectComponents (BitVector& triangles, 
                  Bool manifoldOnly, 
                  unsigned int  * ret_topology_type)
{
  if(triangles.getNumSet()==0) // nothing selected
    return FALSE;

  BitVector visited(triangles.size());
  BitVectorIter iter(triangles);
  OrTriStack s;

  if(ret_topology_type) *ret_topology_type = CLOSED_COMPONENT;
  int numComp = 0;
  for(iter.first(); !iter.isDone(); iter.next(), numComp++)
  {
    OrTri ef(iter.curItem(), 0);
    s.push(ef);
    while(!s.empty())
    {
      ef = s.pop();
      visited.set(ef.index());
      triangles.clear(ef.index());
      for(int i=0; i<3; i++, ef = ef.enext())
      {
        OrTri ef1 = comp_->FnextComplete(ef);

        if(visited.test(ef1.index()))
          continue;

        OrTri ef2 = comp_->FnextComplete(ef1);

        if(ret_topology_type)
        {
          if(ef2.index() != ef.index())
            *ret_topology_type |= HAS_NMEDGES;
          if(ef1.index()==ef.index())
            *ret_topology_type |= HAS_BOUNDARY;
          else if(outside_(ef)==outside_(ef1))
            *ret_topology_type |= NOT_ORIENTED;
        }

        if(manifoldOnly &&
          (ef1.index()==ef.index() || 
           ef2.index() != ef.index()))
          continue;
        
        s.push(ef1);
      }
    }
  }

  if(ret_topology_type && (numComp > 1))
    *ret_topology_type |= MULTIPLE_COMPONENTS;

  triangles = visited;
  return (triangles.getNumSet() != 0);
}
