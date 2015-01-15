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

#ifndef __RGI_ORIENTER_H__
#define __RGI_ORIENTER_H__

#include <geometry/tristmodifier.h>
#include <geometry/geomutil.h>

class RealPArray;
class ComplexEdgeSet;
class ComplexVertexSet;

class TristOrienter : public TristModifier
{
public:
  TristOrienter (Trist * tr, RealPArray * rArray, int flipTriangles = TRUE, int delInterior = TRUE);
  ~TristOrienter (void);
  Bool maintainSets (ComplexEdgeSet * edges, ComplexVertexSet * verts);
  Bool orientNonPrincipalTri (BitVector& triangles);
  Bool orientPrincipalTri (BitVector& principal, BitVector& inComp);
	Bool markPrincipal (BitVector &shadowBV, BitVector & principalBV);
	Bool orientAll (BitVector &shadowBV, BitVector &principalBV);

	BitVector& getReorientBV (void) { return rorBV_; }

private:
  Bool flip_ (FIndex f);
  Bool outside_ (OrTri ef);
  void getNormal_ (OrTri ef,double*);
  OrTri findHighestOrTri_ (BitVector&);
  void setReorient_ (OrTri, BitVector&, BitVector& , BitVector& , BitVector & , OrTriStack &);
  void delInnerTri_ (BitVector& , BitVector & , OrTriStack &);
	void MPDFS_(OrTri tri,BitVector * traversed, BitVector &shadowBV, 
                BitVector & principalBV, BitVector *thistime);
	  //- Mark Principal DFS
  RealPArray * rArray_;
  BitVector rorBV_;
	int flipTriangles_;
	int delInterior_;
  ComplexEdgeSet * edges_;
  ComplexVertexSet * verts_;
};

#endif // __TRISTORIENTER_H__
