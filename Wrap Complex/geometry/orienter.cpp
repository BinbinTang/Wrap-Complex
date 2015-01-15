//
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
//

#include <basic/points.h>
#include <basic/miscmath.h>
#include <geometry/orienter.h>
#include <geometry/geomutil.h>
#include <geometry/edgeset.h>
#include <geometry/vertset.h>


TristOrienter::
TristOrienter (Trist * tr, RealPArray * rArray, int flipTriangles,
							 int delInterior)
: TristModifier(tr), rArray_(rArray), rorBV_ (tr->numTri ()),
  edges_(NULL), verts_(NULL)
{
	flipTriangles_ = flipTriangles;
	delInterior_ = delInterior;
}

TristOrienter::
~TristOrienter (void)
{
  
}

Bool TristOrienter::
maintainSets (ComplexEdgeSet * edges, ComplexVertexSet * verts)
{
  edges_ = edges;
  verts_ = verts;
  if(verts_) verts_->invalidate();
  return TRUE;
}

inline Bool TristOrienter::
outside_ (OrTri ef) // oriented outside by version
{
	int version = ef.version ();
	int reoriented = rorBV_.test (ef.index ());
	return ((!(version & 1)) || reoriented); 
}

void TristOrienter::
getNormal_ (OrTri ef,double* nx)
{
	double ax[4],bx[4],cx[4];
	rArray_->getPoint(Org_(ef), ax);
	rArray_->getPoint(Dest_(ef), bx);
	rArray_->getPoint(Dest_(Enext_(ef)), cx);
	vnorm(vsub(bx, ax));
	vnorm(vsub(cx, ax));
	vnorm(vcross(bx, cx, nx));
}

//--------------------------------------------------------------


Bool TristOrienter::
orientNonPrincipalTri (BitVector& triangles)
{
	BitVector dfsBV(triangles.size()); // traversed
//	BitVector rorBV(triangles.size()); // to be reoriented
	BitVector triBV(triangles);        // working set
	BitVector shlBV(triangles.size());
	BitVector ptdBV(triangles.size());
	OrTriStack ptdOTS;
  progressUpdate(0.1f);
	while (triBV.getNumSet())
	{
		OrTri efRef = findHighestOrTri_ (triBV);
		// efRef points outside geometrically
		setReorient_(efRef, triBV, dfsBV, shlBV, ptdBV , ptdOTS); 
    // This is now configurable -- delete interior can be skipped.
//    if (delInterior_)
//			delInnerTri_(shlBV, ptdBV, ptdOTS);

		triBV.sub_op(ptdBV);
		ptdBV.reset();

		// now dfsBV set to traversed triangles, which is closed component;
		// rorBV stores incorrectly oriented triangles

		// not necessary, but do it for now (for debugging purpose).
		// dfsBV.reset();
		triBV.sub_op(dfsBV);
		// rorBV_.reset();   We now want to maintain this...
	}
  progressUpdate(0.6f);
	// Now flip all triangles that need to be reoriented.
	if (flipTriangles_)
	{
		BitVectorIter rorIter(rorBV_);
		for(rorIter.first(); !rorIter.isDone(); rorIter.next())
		{
			FIndex f = rorIter.curItem();
			flip_(f);
		}
		rorBV_.reset ();
	}
  progressUpdate(0.9f);
	return TRUE;
}

Bool TristOrienter::
orientPrincipalTri (BitVector& principal, BitVector& inComp)
{
  BitVector dfsBV(principal.size());
  OrTriStack s;
  progressUpdate(0.1f);
  BitVectorIter iter(principal);
  for(iter.first();!iter.isDone();iter.next())
  {
    FIndex f = iter.curItem();
    if(!inComp.test(f) || dfsBV.test(f))
      continue;
    
    dfsBV.set(f);
    OrTri ef(f,0),ef1;
    
    s.push(ef);
    
    while(!s.empty())
    {
      ef = s.pop();
      if(!outside_(ef))
        rorBV_.set (ef.index ()); // face inside, has to be reoriented.
      ef = ef.sym(); // flip it to inside, orientationwise.
            
      for(int i=0;i<3;i++,ef=Enext_(ef))
      {
        ef1 = FnextBV_(ef, inComp, FALSE);
        if ((ef1.index() == ef.index()) ||
          (FnextBV_(ef1, inComp, FALSE).index() != ef.index()))
          continue;
                
        if(!dfsBV.test(ef1.index()))
        {
          dfsBV.set(ef1.index());
          s.push(ef1);
        }
      }
    }
  }
  progressUpdate(0.6f);
	// Now flip all triangles that need to be reoriented.
	if (flipTriangles_)
	{
		BitVectorIter rorIter(rorBV_);
		for(rorIter.first(); !rorIter.isDone(); rorIter.next())
		{
			FIndex f = rorIter.curItem();
			flip_(f);
		}
		rorBV_.reset ();
	}
  progressUpdate(0.9f);
  return TRUE;
}

OrTri TristOrienter::
findHighestOrTri_ (BitVector& triBV)
{
  
	OrTri ef, ef1, maxEF;
	double maxZ = -MAXFLOAT;
	double nx[4], n2x[4];
	// find OrTri, representing highest z coord
	{
		BitVector vert(rArray_->size());
		BitVectorIter iter(triBV);
		for(iter.first(); !iter.isDone(); iter.next())
		{
			ef1 = OrTri(iter.curItem(), 0);
			for(int i=0; i<3; i++, ef1 = Enext_(ef1))
			{
				VIndex a =Org_(ef1);
				if (vert.test(a))
					continue; // we go through this vertex
				vert.set(a);
				double ax[4];
				rArray_->getPoint(a, ax);
				if (ax[2] < maxZ)
					continue;
				maxZ = ax[2]; maxEF = ef1;
			}
		}
	}


	// get the maxEF facing upwards.
	getNormal_(maxEF,nx);
	if(nx[2]<0)
	{
		maxEF = Enext_(Sym_(maxEF));
		nx[0] = -nx[0];
		nx[1] = -nx[1];
		nx[2] = -nx[2];
	}

	BitVector visitedBV(triBV.size());


	OrTriStack s;
	s.push(maxEF);

	visitedBV.set(maxEF.index());

	while(!s.empty())
	{
		ef = s.pop();
		getNormal_(ef,n2x);
		if(n2x[2]<0)
		{
			n2x[0] = -n2x[0];
			n2x[1] = -n2x[1];
			n2x[2] = -n2x[2];
			ef = Enext_(Sym_(ef));
		}
		if(n2x[2]>nx[2])
		{
			nx[0] = n2x[0];
			nx[1] = n2x[1];
			nx[2] = n2x[2];
			maxEF = ef;
		}

		for(ef1 = Fnext_(ef); ef1.index()!=ef.index();ef1 = Fnext_(ef1))
//		for(ef1 = FnextBV_(ef,triBV); ef1.index()!=ef.index();ef1 = FnextBV_(ef1,triBV))
			if(!visitedBV.test(ef1.index()))
			{
				if(triBV.test(ef1.index()))
					s.push(ef1);
				visitedBV.set(ef1.index());
			}
		for(ef1 = Fnext_(Enext2_(ef)); ef1.index()!=ef.index();ef1 = Fnext_(ef1))
//		for(ef1 = FnextBV_(Enext2_(ef),triBV); ef1.index()!=ef.index();ef1 = FnextBV_(ef1,triBV))
			if(!visitedBV.test(ef1.index()))
			{
				if(triBV.test(ef1.index()))
					s.push(Enext_(ef1));
				visitedBV.set(ef1.index());
			}
	}

/*	
	do {
		visitedBV.set(maxEF.index());
		maxEFChanged = FALSE;
		ef1 = FnextBV_(maxEF,triBV);
		if(!visitedBV.test(ef1.index()))
			getNormal_(ef1,n2x);
		if(fabs(n2x[2])>=nx[2]&&!visitedBV.test(ef1.index()))
		{
			maxEFChanged = TRUE;
			maxEF = n2x[2] > 0 ? ef1 : Sym_(ef1);
			getNormal_(maxEF,nx);
		}
		else
		{
			ef1 = Enext_(FnextBV_(Enext2_(maxEF),triBV));
			if(!visitedBV.test(ef1.index()))
				getNormal_(ef1,n2x);
			if(fabs(n2x[2])>=nx[2]&&!visitedBV.test(ef1.index()))
			{
				maxEFChanged = TRUE;
				maxEF = n2x[2] > 0 ? ef1 : Sym_(ef1);
				getNormal_(maxEF,nx);
			}
		}
	} while (maxEFChanged);
*/	
	return maxEF; // oriented outside geometrically
}

//----------------------------------------------------------------------------

Bool TristOrienter::
markPrincipal (BitVector &shadowBV, BitVector & principalBV)
{
  BitVector traversed[2];
  BitVector thistime;

  float r = (float) tr_->numTri();
  r = r? 1.0f/r : 1.0f;
  float fraction = 0.0f;

  TristIter ti(*tr_);

  for(ti.first();!ti.isDone();ti.next())
  {
    OrTri tri(ti.curItem(),0);
    if(!traversed[0].test(ti.curItem()))
    {
      thistime.reset();
      MPDFS_(tri,traversed, shadowBV, principalBV, &thistime);
    }
    if(!traversed[1].test(ti.curItem()))
    {
      thistime.reset();
      MPDFS_(tri.sym(),traversed, shadowBV, principalBV, &thistime);
    }
	progressUpdate (fraction += r);
  }
	return TRUE;
}

//----------------------------------------------------------------------------

Bool TristOrienter::
orientAll(BitVector &shadowBV, BitVector &principalBV)
{
  int prog1, prog2, prog3;
  getProgressBounds(prog1, prog3);
  BitVector triBV(tr_->getTriangleBV());
  triBV.sub_op(shadowBV);
  // now leave only non-principal triangles in triBV
  triBV.sub_op(principalBV);

  int all = triBV.getNumSet()+principalBV.getNumSet();
  if (all > 0)
    prog2 = (triBV.getNumSet()*prog1+principalBV.getNumSet()*prog3)/all;
  else
    prog2 = (prog1 + prog3) / 2;
  
  setProgressBounds(prog1, prog2);
  orientNonPrincipalTri(triBV);
  // orienter might delete some triangles, so update shadow and principal.
  shadowBV.and_op(tr_->getTriangleBV());
  principalBV.and_op(tr_->getTriangleBV());

	// Compute all triangles in the complex
  BitVector inCompBV (tr_->getTriangleBV ());
	inCompBV.sub_op (shadowBV);
  // Reorient only the principal ones
  setProgressBounds(prog2, prog3);
	orientPrincipalTri(principalBV, inCompBV);
	return TRUE;
}

//----------------------------------------------------------------------------

void TristOrienter::
MPDFS_(OrTri tri,BitVector * traversed, BitVector &shadowBV, 
                BitVector & principalBV, BitVector *thistime)
{
  OrTri t;

  OrTriStack s;
  s.push(tri);
  while (!s.empty())
  {
    tri = s.pop();
    if(shadowBV.test(tri.index()))
      continue;
    if(traversed[tri.version()%2].test(tri.index()))
      continue;
    traversed[tri.version()%2].set(tri.index());

    if(thistime->test(tri.index()))
      principalBV.set(tri.index());
    else
      principalBV.clear(tri.index());

    thistime->set(tri.index());
    s.push(t = Fnext_(tri).sym());
    s.push(t = Fnext_(tri.enext()).sym());
    s.push(t = Fnext_(tri.enext2()).sym());
  }
}

//----------------------------------------------------------------------------

void TristOrienter::
setReorient_ (OrTri efRef, BitVector& triBV, BitVector& dfsBV,
			  BitVector& shlBV, BitVector& ptdBV, OrTriStack& ptdOTS)
{
    OrTriStack s;
	s.push(efRef);

	shlBV.set(efRef.index());
	Assert(!ptdBV.test(efRef.index()));

	while (!s.empty())
	{
		OrTri ef = s.pop(); // outside geometrically

		FIndex f = ef.index();
		shlBV.set(ef.index());
		ptdBV.clear(ef.index());

		if (dfsBV.test(f))
			continue;

		dfsBV.set(f);
			// set reorientation bitvector
		if (!outside_(ef)) // not outside by version
			rorBV_.set(f);

		for(int i=0; i<3; i++, ef = Enext_(ef))
		{

			OrTri ef1 = Sym_(FnextBV_(ef, triBV));  // oriented outside

			if (dfsBV.test(ef1.index()))
				continue;
			s.push(ef1);

			for(OrTri ef2 = Fnext_(Sym_(ef1));ef2.index()!=ef.index();ef2 = Fnext_(ef2))
				if(!shlBV.test(ef2.index()))
				{
					ptdBV.set(ef2.index());
					ptdOTS.push(ef2);
				}
		}
	}
}

//------------------------------------------------------------------

void TristOrienter::
delInnerTri_ (BitVector& shlBV, BitVector& ptdBV , OrTriStack& s)
{

	OrTri ef,ef1;

	while(!s.empty())
	{
		ef = s.pop();
		if(shlBV.test(ef.index()))
			continue;
		ptdBV.set(ef.index());
		ef = Enext_(ef);
		for(int i=0;i<2;i++, ef = Enext_(ef))
		{

			for(ef1=Fnext_(ef);
			    !ptdBV.test(ef1.index())&&!shlBV.test(ef1.index());ef1=Fnext_(ef1))
				if(!ptdBV.test(ef1.index()))
				{
					ptdBV.set(ef1.index());
					s.push(ef1);
				}
			for(ef1=Fnext_(Sym_(ef));
			    !ptdBV.test(ef1.index())&&!shlBV.test(ef1.index());ef1=Fnext_(ef1))
				if(!ptdBV.test(ef1.index()))
				{
					ptdBV.set(ef1.index());
					s.push(ef1);
				}
		}
	}
  
	DelTri_(ptdBV);

}

//-------------------------------------------------------------------------------

Bool TristOrienter::
flip_ (FIndex f)
{
  Flip_(f);
  if(edges_)
  {
    OrTri ef(f, 0);
    int rep1 = ERep_(ef.enext()), rep2 = ERep_(ef.enext2());
    BitVector &eBV = edges_->getBitVector();
    Bool marked1 = eBV.test(rep1), marked2 = eBV.test(rep2);
    if(marked1) eBV.clear(rep1);
    if(marked2) eBV.clear(rep2);
    if(marked1) eBV.set(rep2);
    if(marked2) eBV.set(rep1);
  }

  if(verts_) // do it on flipped triangle
  {
    OrTri ef(f, 0);
    Bool isASet = verts_->test(ef.sym()),
      isBSet = verts_->test(ef);

    if(isASet) verts_->clear(ef.sym());
    if(isBSet) verts_->clear(ef);

    if(isASet) verts_->set(ef);
    if(isBSet) verts_->set(ef.sym());
  }
  return TRUE;
}
