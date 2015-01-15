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

#ifndef __RGI_COMPUTIL_H__
#define __RGI_COMPUTIL_H__

#include <basic/rgitypes.h>
#include <geometry/comp.h>

class RectSelection;

class Complex_SelectUtil : public TristModifier
{
public:
  Complex_SelectUtil (Complex * comp);
	Bool getPointOnEdge(OrTri ef, RectSelection& rectSel, double dx[3]);
	Bool getPointOnTri(OrTri ef, RectSelection& rectSel, double dx[3], Bool exactPointOnTri = FALSE);
	OrTri getClosestVertexOrTri (RectSelection& rectSel, int fInd);
	OrTri getClosestEdgeOrTri (RectSelection& rectSel, int fInd);
	OrTri getOrTri (RectSelection& rectSel, int fInd);
	OrTriStack * getOrTri (RectSelection& rectSel, IndexStack & iStack);
	OrTriStack * getOrTri (RectSelection& rectSel, BitVector & bv);
	void markRectanglePoints (RectSelection& rectSel, BitVector& triBV,
					 BitVector& vertBV, OrTriStack * efStack);
  Bool selectComponents (BitVector &triangles, 
                        Bool manifoldOnly, 
                        unsigned int  * ret_topology_type = NULL);
private:
  Complex * comp_;
}; 


#endif // __COMPLEX_SELECT_UTIL_H__